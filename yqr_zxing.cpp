#include <cstdlib>
#include <cstring>
#include <string>

#include "ZXingC.h"

extern "C" {

char* yqr_decode_gray(
    const unsigned char* data,
    int width,
    int height
) {
    if (!data || width <= 0 || height <= 0)
        return nullptr;

    ZXing_ImageView* image = ZXing_ImageView_new_checked(
        data,
        width * height,
        width,
        height,
        ZXing_ImageFormat_Lum,
        width,
        1
    );

    if (!image)
        return nullptr;

    ZXing_ReaderOptions* opts = ZXing_ReaderOptions_new();

    if (!opts) {
        ZXing_ImageView_delete(image);
        return nullptr;
    }

    ZXing_BarcodeFormat format =
        ZXing_BarcodeFormatFromString("QRCode");

    ZXing_ReaderOptions_setFormats(opts, &format, 1);
    ZXing_ReaderOptions_setTryHarder(opts, true);
    ZXing_ReaderOptions_setTryRotate(opts, true);
    ZXing_ReaderOptions_setTryInvert(opts, true);

    ZXing_Barcodes* results =
        ZXing_ReadBarcodes(image, opts);

    char* output = nullptr;

    if (results) {
        int count = ZXing_Barcodes_size(results);

        for (int i = 0; i < count; ++i) {
            const ZXing_Barcode* barcode =
                ZXing_Barcodes_at(results, i);

            if (!barcode)
                continue;

            char* text = ZXing_Barcode_text(barcode);

            if (!text)
                continue;

            if (std::strncmp(text, "YQR1|", 5) == 0) {
                size_t len = std::strlen(text);

                output = static_cast<char*>(
                    std::malloc(len + 1)
                );

                if (output)
                    std::memcpy(output, text, len + 1);

                ZXing_free(text);
                break;
            }

            ZXing_free(text);
        }

        ZXing_Barcodes_delete(results);
    }

    ZXing_ReaderOptions_delete(opts);
    ZXing_ImageView_delete(image);

    return output;
}

void yqr_free(char* ptr)
{
    std::free(ptr);
}

}
