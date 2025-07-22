#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <iostream>

#define SIZE 16
#define K 4
#define SIZE_OUT (SIZE / K)

typedef ap_axis<32, 0, 0, 0> axis_t;

void axis_bram(hls::stream<axis_t> &in_stream,
               hls::stream<axis_t> out_stream[K]);

int main() {
    hls::stream<axis_t> in_stream;
    hls::stream<axis_t> out_stream[K];

    
    for (int i = 1; i <= SIZE; ++i) {
        axis_t val;
        val.data = i;
        val.keep = 0xF;
        val.strb = 0xF;
        val.last = (i == SIZE) ? 1 : 0;
        in_stream.write(val);
    }

    
    axis_bram(in_stream, out_stream);

  
    for (int i = 0; i < SIZE_OUT; ++i) {
        std::cout << "Grupo " << i << ": ";
        for (int j = 0; j < K; ++j) {
            if (!out_stream[j].empty()) {
                axis_t val = out_stream[j].read();
                std::cout << val.data.to_int() << " ";
            } else {
                std::cout << "[VACÍO] ";
            }
        }
        std::cout << std::endl;
    }

    return 0;
}
