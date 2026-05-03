#pragma once
#include <string>
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>
#include <wrl/client.h>
#include <filesystem>

using Microsoft::WRL::ComPtr;

class ShaderCompiler {

public:
    ComPtr<ID3DBlob> compileOrLoad(const std::string& hlslPath, const char* entry, const char* profile) {
        return compile(hlslPath, entry, profile);
    }

private:
    std::string readShader(const std::string& filename) {
        std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    ComPtr<ID3DBlob> compile(const std::string& filePath, const char* entry, const char* profile) {
        std::string src = readShader(filePath);
        
        ComPtr<ID3DBlob> shader, error;

        HRESULT hr = D3DCompile(
            src.c_str(),
            strlen(src.c_str()),
            nullptr, nullptr, nullptr,
            entry, profile,
            0, 0,
            &shader, &error
        );

        if (FAILED(hr)) {
            OutputDebugStringA((char*) error->GetBufferPointer());
            return nullptr;
        }
        return shader;
    }
};
