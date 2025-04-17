#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <opencv2/opencv.hpp>

typedef __int128 int128_t;
using namespace std::string_literals;

// A. Uncomment this to run assembly code.
/*
extern "C" unsigned render(unsigned int W, unsigned int H, uint8_t *pBuffer, int64_t mid_x, int64_t mid_y, int64_t span, uint32_t color);
*/

// B. Uncomment this to run demo with zooming functionality.

void render(int64_t W, int64_t H, uint8_t *pBuffer, int64_t mid_x, int64_t mid_y, int64_t span, uint32_t color) {
    for (int row = 0; row < H; row++) {
        for (int col = 0; col < W; col++) {
            uint8_t *p = pBuffer + 3 * (row * W + col);

            int64_t x = mid_x + static_cast<int64_t>(static_cast<int128_t>(span) * static_cast<int128_t>(2 * col - W) / static_cast<int128_t>(2 * W));
            int64_t y = mid_y + static_cast<int64_t>(static_cast<int128_t>(span) * static_cast<int128_t>((2 * row - H) * H) / static_cast<int128_t>(2 * H * W));

            if ((std::abs(x) < 0x0800000000000000l) && (std::abs(y) < 0x0800000000000000l)) {
                *(p + 0) = 255u;
                *(p + 1) = 255u;
                *(p + 2) = 255u;
            } else {
                *(p + 0) = 0u;
                *(p + 1) = 0u;
                *(p + 2) = 0u;
            }
        }
    }
}


// Image size.
int64_t g_w{512}, g_h{512};

// Midpoint of the currently displayed area.
int64_t g_x = 0; // Q5.59, range [-16, 16)
int64_t g_y = 0; // -||-

// Horizontal span of the currently displayed area.
constexpr int64_t SPAN_MAX = 0x4000000000000000l; // 8.0 in Q5.59, range [-16, 16)
constexpr int64_t SPAN_MIN = 0x0000000000000001l; // -||-
int64_t g_span = SPAN_MAX;

// Custom param.
uint32_t g_color = 0xFF0000u /* red */ /* | 0x00FF00u */ /* green */ | 0x0000FFu /* blue */;

// Zoom in / zoom out functionality.
void update(int event, int x, int y, int flags, void *pUser) {
    if (cv::EVENT_MOUSEWHEEL == event) {
        int64_t span = g_span;
        if (0 < flags) {
            span >>= (SPAN_MIN < span)?(1):(0);
        } else {
            span <<= (SPAN_MAX > span)?(1):(0);
        }
    
        if (span != g_span) {
            if (SPAN_MAX == span) {
                g_x = g_y = 0;
                
            } else {
                int64_t x_ = x;
                int64_t y_ = y;
            
                g_x += static_cast<int64_t>(static_cast<int128_t>(g_span - span) * static_cast<int128_t>(2 * x_ - g_w) / static_cast<int128_t>(2 * g_w));
                g_y -= static_cast<int64_t>(static_cast<int128_t>(g_span - span) * static_cast<int128_t>((g_h - 2 * y_) * g_h) / static_cast<int128_t>(2 * g_h * g_w));
            }
        }
        g_span = span;
        
        cv::Mat *pMat = static_cast<cv::Mat*>(pUser);
        render(g_w, g_h, static_cast<uint8_t*>(pMat->ptr()), g_x, g_y, g_span, g_color);
        cv::imshow("Demo", *pMat);
    }
}

/**
 * Usage:
 * "./demo"                     - runs in batch mode,       output file is default ("out.png"), size is default (WxH - 512x512),
 * "./demo out2.png"            - runs in batch mode,       output file is "out2.png", size is default (512x512),
 * "./demo out2.png 100"        - runs in batch mode,       output file is "out2.png", width is 100, height is default (512),
 * "./demo out2.png 100 200"    - runs in batch mode,       output file is "out2.png", width is 100, height is 200,
 * "./demo "" 100"              - runs in interactive mode, width is 100, height is default (512),
 * "./demo "" 100 200"          - runs in interactive mode, width is 100, height is 200,
 */
int main(int argc, char *argv[]) {
    std::string path{"out.png"s};

    // Width
    if (2 <= argc) {
        path = argv[1];
    }

    // Width
    if (3 <= argc) {
        int64_t w = std::atoi(argv[2]);
        if (0 < w) {
            g_w = w;
        }
    }
    
    // Height
    if (4 <= argc) {
        int64_t h = std::atoi(argv[3]);
        if (0 < h) {
            g_h = h;
        }
    }
    g_h = std::min(g_h, (2 * g_w) - 1);

    // Here add custom param(s)..

    // Color.
    if (5 <= argc) {
        int64_t c = std::atoi(argv[3]);
        if (0 < c) {
            g_color = c;
        }
    }
    
    // Midpoint & zoom can be modified here too.

    // ..end adding custom params.
    
    cv::Mat mat(g_w, g_h, CV_8UC3);
    if (path.empty()) {
        std::cout << "Interactive mode ("s << g_w << "x"s << g_h << ")."s << std::endl;
        
        render(g_w, g_h, static_cast<uint8_t*>(mat.ptr()), g_x, g_y, g_span, g_color);
        
        cv::namedWindow("Demo", cv::WINDOW_OPENGL);
        cv::resizeWindow("Demo", g_w, g_h);
        cv::setMouseCallback("Demo", &update, static_cast<void*>(&mat));
        cv::imshow("Demo", mat);
        
        while(cv::getWindowProperty("Demo", cv::WND_PROP_VISIBLE) && (27 != cv::waitKey(10))) {
            // empty
        }

    } else {
        std::cout << "Batch mode (\""s << path << "\", "s << g_w << "x"s << g_h << ")."s << std::endl;
        
        render(g_w, g_h, static_cast<uint8_t*>(mat.ptr()), g_x, g_y, g_span, g_color);
        cv::imwrite(path, mat);
    }
    
    std::cout << "Done."s << std::endl;

    return 0;
}

