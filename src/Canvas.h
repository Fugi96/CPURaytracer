#pragma once
#include <cstdint>
#include <vector>
#include "util/Color.h"
#include <string>
#include <fstream>

class Canvas {
public:
    uint16_t m_height;
    uint16_t m_width;
    bool reading{ false };

    Canvas(uint16_t width, uint16_t height)
        : m_height(height), 
        m_width(width), 
        m_pixels(width * height, 
        Color(0.0f, 0.0f, 0.0f)) 
    {
    }

    void write_pixel(uint16_t x, uint16_t y, const Color& c) {        
        m_pixels[(static_cast<size_t>(y) * static_cast<size_t>(m_width)) + x] = c;
    }

    const Color& pixel_at(uint16_t x, uint16_t y) const {
        return m_pixels[(static_cast<size_t>(y) * static_cast<size_t>(m_width)) + x];
    }

    void reset_image() {
        m_pixels.assign(m_width * m_height, Color(0.0f, 0.0f, 0.0f)); 
    }

    const std::string exportToPPM() const {
        std::string outString = "P3\n" +
            std::to_string(m_width) + " " +
            std::to_string(m_height) + "\n" +
            "255\n";

        for (uint16_t y = 0; y < m_height; ++y) {
            std::string line = "";
            uint8_t lineLength = 0;
            for (uint16_t x = 0; x < m_width; ++x) {
                Color c = pixel_at(x, y);
                glm::vec3 colorOut = c.outValue();
                appendColorValue(line, colorOut.r, lineLength);
                appendColorValue(line, colorOut.g, lineLength);
                appendColorValue(line, colorOut.b, lineLength);
            }
            line.erase(0, 1);
            line += "\n";
            outString += line;
        }
        return outString;
    }

    std::vector<Color> texture() {
        return this->m_pixels;
    }

    static void saveToFile(const std::string& name, const std::string& ppm) {
        std::ofstream out(name + ".ppm");
        out << ppm;
        out.close();
    }


private:
    std::vector<Color> m_pixels;

    inline void appendColorValue(std::string& line, float& value, uint8_t& length) const {
        uint8_t intValue = static_cast<uint8_t>(value);
        if (length + std::to_string(intValue).size() > 69) {
            line.append("\n" + std::to_string(intValue));
            length = 0;
        }
        else {
            line.append(" " + std::to_string(intValue));
            length = length + static_cast<uint8_t>(std::to_string(intValue).size()) + 1;
        }
    }
};