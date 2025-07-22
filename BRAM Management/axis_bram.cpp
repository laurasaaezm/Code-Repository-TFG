#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <ap_int.h>

#define SIZE 16         // Vector of probabilities  
#define K 4             // Configurable parallel factor
#define SIZE_OUT (SIZE / K)

typedef ap_axis<32, 0, 0, 0> axis_t;

void axis_bram(hls::stream<axis_t> &in_stream,
               hls::stream<axis_t> out_stream[K]) {
#pragma HLS INTERFACE axis port=in_stream
#pragma HLS INTERFACE axis port=out_stream
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL

#pragma HLS ARRAY_PARTITION variable=out_stream complete dim=1

    static ap_uint<32> bram[SIZE];
#pragma HLS BIND_STORAGE variable=bram type=ram_2p impl=bram
#pragma HLS ARRAY_PARTITION variable=bram cyclic factor=K dim=1
#pragma HLS DEPENDENCE variable=bram inter false

read_loop:
    for (int i = 0; i < SIZE; i++) {
#pragma HLS PIPELINE II=1
        axis_t val = in_stream.read();
        bram[i] = val.data;
    }

write_loop:
    for (int i = 0; i < SIZE_OUT; i++) {
#pragma HLS PIPELINE II=1
        for (int j = 0; j < K; j++) {
#pragma HLS UNROLL
            int idx = i * K + j;
            axis_t val;
            val.data = bram[idx];
            val.keep = 0xF;
            val.strb = 0xF;
            val.last = (idx == SIZE - 1) ? 1 : 0;
            out_stream[j].write(val); 
    }
}

