#include "manifest.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

#define JSMN_STATIC
#define JSMN_STRICT
#include "../../third_party/jsmn/jsmn.h"

namespace amethisty {
namespace {

constexpr std::size_t kMaxManifestBytes = 64 * 1024;
constexpr unsigned int kMaxTokens = 4096;
constexpr char kSupportedGame[] = "2.2.145";

void appendUtf8(std::string& out, std::uint32_t cp) {
    if (cp <= 0x7F) {
        out.push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FF) {
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
}

bool hex4(const std::string& input, std::size_t pos, std::uint32_t& value) {
    if (pos + 4 > input.size()) return false;
    value = 0;
    for (std::size_t i = 0; i < 4; ++i) {
        const char c = input[pos + i];
        unsigned int digit = 0;
        if (c >= '0' && c <= '9') digit = static_cast<unsigned int>(c - '0');
        else if (c >= 'a' && c <= 'f') digit = 10u + static_cast<unsigned int>(c - 'a');
        else if (c >= 'A' && c <= 'F') digit = 10u + static_cast<unsigned int>(c - 'A');
        else return false;
        value = (value << 4) | digit;
    }
    return true;
}

bool decodeString(const std::string& input, const jsmntok_t& token,
                  std::string& output) {
    output.clear();
    for (std::size_t i = static_cast<std::size_t>(token.start);
         i < static_cast<std::size_t>(token.end); ++i) {
        const unsigned char c = static_cast<unsigned char>(input[i]);
        if (c != '\\') {
            output.push_back(static_cast<char>(c));
            continue;
        }
        if (++i >= static_cast<std::size_t>(token.end)) return false;
        switch (input[i]) {
            case '"': output.push_back('"'); break;
            case '\\': output.push_back('\\'); break;
            case '/': output.push_back('/'); break;
            case 'b': output.push_back('\b'); break;
            case 'f': output.push_back('\f'); break;
            case 'n': output.push_back('\n'); break;
            case 'r': output.push_back('\r'); break;
            case 't': output.push_back('\t'); break;
            case 'u': {
                std::uint32_t cp = 0;
                if (!hex4(input, i + 1, cp)) return false;
                i += 4;
                if (cp >= 0xD800 && cp <= 0xDBFF) {
                    if (i + 6 >= static_cast<std::size_t>(token.end) ||
                        input[i + 1] != '\\' || input[i + 2] != 'u') return false;
                    std::uint32_t low = 0;
                    if (!hex4(input, i + 3, low) || low < 0xDC00 || low > 0xDFFF) return false;
                    cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                    i += 6;
                } else if (cp >= 0xDC00 && cp <= 0xDFFF) {
                    return false;
                }
                appendUtf8(output, cp);
                break;
            }
            default: return false;
        }
    }
    return true;
}

int nextToken(const std::vector<jsmntok_t>& tokens, int index) {
    const int end = tokens[static_cast<std::size_t>(index)].end;
    ++index;
    while (index < static_cast<int>(tokens.size()) &&
           tokens[static_cast<std::size_t>(index)].start < end) {
        ++index;
    }
    return index;
}

bool isNonEmpty(const std::string& value) {
    return !value.empty() && std::any_of(value.begin(), value.end(), [](unsigned char c) {
        return !std::isspace(c);
    });
}

bool validId(const std::string& id) {
    if (id.size() < 3 || id.find('.') == std::string::npos) return false;
    bool segmentHasChar = false;
    for (std::size_t i = 0; i < id.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(id[i]);
        if (c == '.') {
            if (!segmentHasChar || i + 1 == id.size()) return false;
            segmentHasChar = false;
            continue;
        }
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
            segmentHasChar = true;
            continue;
        }
        if (c == '-' && segmentHasChar && i + 1 < id.size() &&
            id[i + 1] != '.' && id[i + 1] != '-') {
            continue;
        }
        return false;
    }
    return segmentHasChar;
}

bool safeLibraryPath(const std::string& path) {
    if (path.size() <= 7 || path.compare(0, 4, "mod/") != 0 ||
        path.front() == '/' || path.find('\\') != std::string::npos ||
        path.compare(path.size() - 3, 3, ".so") != 0) return false;

    std::size_t begin = 0;
    while (begin < path.size()) {
        const std::size_t slash = path.find('/', begin);
        const std::size_t end = slash == std::string::npos ? path.size() : slash;
        const std::string part = path.substr(begin, end - begin);
        if (part.empty() || part == "." || part == "..") return false;
        begin = end + 1;
    }
    return true;
}

bool findField(const std::string& json, const std::vector<jsmntok_t>& tokens,
               const std::string& key, int& valueIndex, bool& duplicate) {
    valueIndex = -1;
    duplicate = false;
    int index = 1;
    while (index < static_cast<int>(tokens.size()) &&
           tokens[static_cast<std::size_t>(index)].start <
               tokens[0].end - 1) {
        const int keyIndex = index;
        const int candidateValue = keyIndex + 1;
        std::string decodedKey;
        if (tokens[static_cast<std::size_t>(keyIndex)].type != JSMN_STRING ||
            candidateValue >= static_cast<int>(tokens.size()) ||
            !decodeString(json, tokens[static_cast<std::size_t>(keyIndex)], decodedKey)) {
            return false;
        }
        if (decodedKey == key) {
            if (valueIndex != -1) duplicate = true;
            valueIndex = candidateValue;
        }
        index = nextToken(tokens, candidateValue);
    }
    return true;
}

bool readStringField(const std::string& json, const std::vector<jsmntok_t>& tokens,
                     const char* key, std::string& value, bool required,
                     std::string& error) {
    int index = -1;
    bool duplicate = false;
    if (!findField(json, tokens, key, index, duplicate) || duplicate) {
        error = std::string("Invalid or duplicate manifest key: ") + key;
        return false;
    }
    if (index == -1) {
        if (!required) {
            value.clear();
            return true;
        }
        error = std::string("Missing required manifest key: ") + key;
        return false;
    }
    if (tokens[static_cast<std::size_t>(index)].type != JSMN_STRING ||
        !decodeString(json, tokens[static_cast<std::size_t>(index)], value) ||
        (required && !isNonEmpty(value))) {
        error = std::string("Manifest key must be a non-empty string: ") + key;
        return false;
    }
    return true;
}

} // namespace

bool parseManifest(const std::string& json, ModManifest& manifest,
                   std::string& error) {
    error.clear();
    manifest = ModManifest{};
    if (json.empty() || json.size() > kMaxManifestBytes) {
        error = "Manifest is empty or exceeds 64 KiB.";
        return false;
    }

    std::vector<jsmntok_t> tokens(kMaxTokens);
    jsmn_parser parser;
    jsmn_init(&parser);
    const int count = jsmn_parse(&parser, json.c_str(), json.size(),
                                 tokens.data(), kMaxTokens);
    if (count < 1) {
        error = "Malformed JSON or manifest token limit exceeded.";
        return false;
    }
    tokens.resize(static_cast<std::size_t>(count));
    if (tokens[0].type != JSMN_OBJECT ||
        static_cast<std::size_t>(tokens[0].end) != json.find_last_not_of(" \t\r\n") + 1) {
        error = "Manifest root must be one JSON object.";
        return false;
    }

    int formatIndex = -1;
    bool duplicate = false;
    if (!findField(json, tokens, "format", formatIndex, duplicate) ||
        duplicate || formatIndex < 0 ||
        tokens[static_cast<std::size_t>(formatIndex)].type != JSMN_PRIMITIVE ||
        json.substr(static_cast<std::size_t>(tokens[static_cast<std::size_t>(formatIndex)].start),
                    static_cast<std::size_t>(tokens[static_cast<std::size_t>(formatIndex)].end -
                                             tokens[static_cast<std::size_t>(formatIndex)].start)) != "1") {
        error = "Manifest format must be integer 1.";
        return false;
    }
    manifest.format = 1;

    if (!readStringField(json, tokens, "id", manifest.id, true, error) ||
        !readStringField(json, tokens, "name", manifest.name, true, error) ||
        !readStringField(json, tokens, "version", manifest.version, true, error) ||
        !readStringField(json, tokens, "author", manifest.author, false, error) ||
        !readStringField(json, tokens, "description", manifest.description, false, error) ||
        !readStringField(json, tokens, "game", manifest.game, true, error) ||
        !readStringField(json, tokens, "library", manifest.library, true, error)) {
        return false;
    }

    if (!validId(manifest.id)) {
        error = "Mod ID must be lowercase reverse-domain style (for example, author.examplemod).";
        return false;
    }
    if (manifest.game != kSupportedGame) {
        error = "This mod targets a different Geometry Dash version; expected 2.2.145.";
        return false;
    }
    if (!safeLibraryPath(manifest.library)) {
        error = "Library path must be a safe relative path under mod/ and end in .so.";
        return false;
    }
    return true;
}

} // namespace amethisty
