#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>
#include <iostream>

#define NUM_PACKETS 100
#define SEED 19660809u
#define TAPS 0xd0000001u

typedef ap_uint<32> AXI_VALUE;
typedef ap_axis<64,2,5,6> AXIS_PACKET;
typedef hls::stream<AXIS_PACKET> AXIS_STREAM;


void top_module(AXIS_STREAM& input_stream, AXIS_STREAM& output_stream);

int main() {
    AXIS_STREAM input_stream, output_stream;

    // Send 10 input packets
    for (int i = 0; i < NUM_PACKETS; i++) {
        AXIS_PACKET pkt;
        pkt.data = i;          
        pkt.keep = -1;         
        pkt.strb = -1;
        pkt.last = (i == NUM_PACKETS - 1) ? 1 : 0;
        pkt.id = 0;
        pkt.dest = 0;
        pkt.user = 0;

        input_stream.write(pkt);
    }

    
    top_module(input_stream, output_stream);

    // Read and show results
    std::cout << "Valores aleatorios generados:\n";
    for (int i = 0; i < NUM_PACKETS; i++) {
        if (!output_stream.empty()) {
            AXIS_PACKET out_pkt = output_stream.read();
            std::cout << "[" << i << "] = " << out_pkt.data << std::endl;
        }
    }

    return 0;
}
