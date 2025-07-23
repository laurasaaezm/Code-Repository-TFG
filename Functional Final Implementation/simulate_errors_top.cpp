#define AP_INT_MAX_W 32768
#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <ap_int.h>

#define SIZE 224
#define K 4
#define SIZE_OUT (SIZE / K)

#define SEED 19660809u
#define TAPS 0xd0000001u

typedef ap_axis<64, 2, 5, 6> axis_in_t;
typedef ap_axis<64 * K, 2, 5, 6> axis_out_t;


void simulate_noise(const ap_uint<64> bram[SIZE],
                    hls::stream<axis_out_t> &out_stream,
                    int current_iter,
                    int total_iters) {
#pragma HLS INLINE off

    ap_uint<32> lfsr_states[K];
#pragma HLS ARRAY_PARTITION variable=lfsr_states complete dim=1

    init_lfsr:
    for (int i = 0; i < K; i++) {
    #pragma HLS UNROLL
        lfsr_states[i] = SEED ^ (i * 0x9E3779B9) ^ (current_iter * 0xABCDEF);
    }

    main_loop:
    for (int i = 0; i < SIZE_OUT; i++) {
    #pragma HLS PIPELINE II=1
        ap_uint<64 * K> packed_output = 0;

        process_k:
        for (int j = 0; j < K; j++) {
        #pragma HLS UNROLL
            int idx = i * K + j;
            ap_uint<64> data = bram[idx];

            ap_uint<32> prob = data.range(63, 32);
            bool bit_entrada = data[0];

            ap_uint<32> &n = lfsr_states[j];
            n = (n >> 1) ^ ((n & 1u) ? TAPS : 0u);

            bool resultado = (n < prob);
            bool bit_salida = bit_entrada ^ resultado;

            ap_uint<64> result = 0;
            result.range(63, 32) = n;
            result[0] = bit_salida;

            packed_output.range((j + 1) * 64 - 1, j * 64) = result;
        }

        axis_out_t out_pkt;
        out_pkt.data = packed_output;
        out_pkt.keep = -1;
        out_pkt.strb = -1;
        out_pkt.last = ((current_iter == total_iters - 1) && (i == SIZE_OUT - 1)) ? 1 : 0;

        out_stream.write(out_pkt);
    }
}


void simulate_errors_top(hls::stream<axis_in_t> &in_stream,
                         hls::stream<axis_out_t> &out_stream,
                         bool update_bram,
                         int num_iter) {
#pragma HLS INTERFACE axis port=in_stream
#pragma HLS INTERFACE axis port=out_stream
#pragma HLS INTERFACE s_axilite port=update_bram bundle=CTRL
#pragma HLS INTERFACE s_axilite port=num_iter bundle=CTRL
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL

    static ap_uint<64> bram[SIZE];
#pragma HLS BIND_STORAGE variable=bram type=ram_2p impl=bram
#pragma HLS ARRAY_PARTITION variable=bram cyclic factor=K dim=1

    if (update_bram) {
        for (int i = 0; i < SIZE; i++) {
        #pragma HLS PIPELINE II=1
            bram[i] = in_stream.read().data;
        }
    }

    for (int it = 0; it < num_iter; it++) {
        simulate_noise(bram, out_stream, it, num_iter);
    }
}
