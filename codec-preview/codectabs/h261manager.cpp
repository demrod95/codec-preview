#include "h261manager.h"

H261Manager::H261Manager() {
    streamingParameters =
        "-c:v h261 -filter:v scale=352:288:force_original_aspect_ratio=decrease,pad=352:288:(ow-iw)/2:(oh-ih)/2 -f matroska";
}
