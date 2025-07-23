#define AP_INT_MAX_W 32768
#include <iostream>
#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>

#define SIZE 221
#define K  4
#define PROB_0_5 2147483647
#define FIXED_NUM_ITER 10000

typedef ap_axis<64, 2, 5, 6> axis_in_t;
typedef ap_axis<64*K, 2, 5, 6> axis_out_t;

void simulate_errors_top(hls::stream<axis_in_t> &in_stream,
                         hls::stream<axis_out_t> &out_stream);

int main() {
    hls::stream<axis_in_t> in_stream;
    hls::stream<axis_out_t> out_stream;

    std::cout << "Inicializando BRAM con " << SIZE << " entradas...\n";

    for (int i = 0; i < SIZE; i++) {
        axis_in_t pkt;
        ap_uint<64> data = 0;
        data.range(63, 32) = PROB_0_5;
        data[0] = 0;

        pkt.data = data;
        pkt.keep = 0xFF;
        pkt.strb = 0xFF;
        pkt.last = (i == SIZE - 1) ? 1 : 0;

        in_stream.write(pkt);
    }

    simulate_errors_top(in_stream, out_stream);

    std::cout << "\nLeyendo resultados (" << FIXED_NUM_ITER << " iteraciones):\n";

    for (int iter = 0; iter < FIXED_NUM_ITER; iter++) {
        std::cout << "\n[Iteración " << iter << "]\n";
        for (int i = 0; i < SIZE / K; i++) {
            axis_out_t pkt = out_stream.read();
            std::cout << "Paquete " << i << ":\n";

            for (int j = 0; j < K; j++) {
                ap_uint<64> result = pkt.data.range((j + 1) * 64 - 1, j * 64);
                ap_uint<32> rand_num = result.range(63, 32);
                bool bit_salida = result[0];

                std::cout << "  [" << j << "] rand = " << rand_num
                          << ", bit_salida = " << bit_salida << "\n";
            }
        }
    }

    return 0;
}
