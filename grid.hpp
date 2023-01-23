#pragma once

#include <iostream>
#include <map>
#include <vector>

template <typename T> class grid {
    // TODO: flattened grid rep? probably not necessary, but still -- maybe?
    std::vector<std::vector<T>> data;
    std::map<T, char> translation;

 public:
    inline void set_translation(const std::map<T, char> &new_trans) {
        translation = new_trans;
    }

    grid(int height, int width) {
        data = std::vector<std::vector<T>>(height, std::vector<T>(width));
    }

    std::vector<T> &operator[](int idx) { return data[idx]; }

    friend std::ostream &operator<<(std::ostream &os, const grid<T> &obj) {
        for (const std::vector<T> &row : obj.data) {
            // TODO: indent?
            for (const T &item : row) {
                std::cout << obj.translation.at(item);
            }
            std::cout << "\n";
        }
        return os;
    }

    inline char translate(T val) { return translation[val]; }

    auto begin() { return std::begin(data); }
    auto end() { return std::end(data); }

    inline size_t height() { return data.size(); }
    inline size_t width() { return data.at(0).size(); }
};
