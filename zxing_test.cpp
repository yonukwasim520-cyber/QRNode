#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "../zxing-build/core/src/ZXingC.h"

static bool read_file(const char* path, std::vector<uint8_t>& data)
{
    std::ifstream f(path, std::ios::binary);
    if (!f)
        return false;

    f.seekg(0, std::ios::end);
    auto size = f.tellg();
    f.seekg(0, std::ios::beg);

    data.resize(static_cast<size_t>(size));
    f.read(reinterpret_cast<char*>(data.data()), size);

    return true;
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        std::cerr << "Usage: zxing_test IMAGE RAW_WIDTH RAW_HEIGHT\n";
        return 1;
    }

    const char* path = argv[1];
    int width = std::stoi(argv[2]);
    int height = std::stoi(argv[3]);

    std::vector<uint8_t> data;

    if (!read_file(path, data)) {
        std::cerr << "Cannot read image\n";
        return 1;
    }

    ZXing_ImageView* image =
        ZXing_ImageView_new_checked(
            data.data(),
            static_cast<int>(data.size()),
            width,
            height,
            ZXing_ImageFormat_Lum,
            width,
            1
        );

    if (!image) {
        std::cerr << "Cannot create ImageView\n";
        return 1;
    }

    ZXing_ReaderOptions* opts = ZXing_ReaderOptions_new();

    ZXing_BarcodeFormat qr =
        ZXing_BarcodeFormatFromString("QRCode");

    ZXing_ReaderOptions_setFormats(opts, &qr, 1);

    ZXing_ReaderOptions_setTryHarder(opts, true);
    ZXing_ReaderOptions_setTryRotate(opts, true);
    ZXing_ReaderOptions_setTryInvert(opts, true);

    ZXing_Barcodes* results =
        ZXing_ReadBarcodes(image, opts);

    if (!results) {
        std::cerr << "ZXing error: ";

        char* error = ZXing_LastErrorMsg();
        if (error) {
            std::cerr << error;
            ZXing_free(error);
        }

        std::cerr << "\n";

        ZXing_ReaderOptions_delete(opts);
        ZXing_ImageView_delete(image);
        return 1;
    }

    int count = ZXing_Barcodes_size(results);

    std::cout << "FOUND=" << count << "\n";

    for (int i = 0; i < count; ++i) {
        const ZXing_Barcode* barcode =
            ZXing_Barcodes_at(results, i);

        char* text = ZXing_Barcode_text(barcode);

        if (text) {
            std::cout << "QR[" << i << "]=" << text << "\n";
            ZXing_free(text);
        }
    }

    ZXing_Barcodes_delete(results);
    ZXing_ReaderOptions_delete(opts);
    ZXing_ImageView_delete(image);

    return 0;
}
