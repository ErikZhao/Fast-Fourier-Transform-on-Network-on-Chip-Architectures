#include <time.h>
#include <systemc.h>

#include "router.h"
#include "pe.h"

using std::string;

int i = 1;

SC_MODULE(top)
{
public:
	enum {N = 3};   //N value of Horizontal 
	enum {M = 3};	//M value of Vertical
	
	router *routers[N][M];	// *router N by M
	PE_base *pes[N][M];		// *PE N by M

	sc_signal<packet> router_to_pe[N][M], pe_to_router[N][M];
	sc_signal<packet> router_to_router_east[N][M-1], router_to_router_west[N][M-1];
	sc_signal<packet> router_to_router_north[N-1][M], router_to_router_south[N-1][M];
	
	sc_signal<packet> terminal_loop_east[N], terminal_loop_west[N];
	sc_signal<packet> terminal_loop_north[M], terminal_loop_south[M];
	sc_signal<bool> clock;

	SC_CTOR(top)
	{
		create_pes();
		create_network();
	}

protected:
	void create_pes()  //*create 9 PEs for new architecture (P0 to P8, PI/PO)
	{
		pes[0][0] = new PE_P3("P3");
		pes[0][0]->clock(clock);
		pes[0][0]->set_xy(0, 0);

		pes[1][0] = new PE_P1("P1");
		pes[1][0]->clock(clock);
		pes[1][0]->set_xy(0, 1);

		pes[2][0] = new PE_P8("P8");
		pes[2][0]->clock(clock);
		pes[2][0]->set_xy(0, 2);

		pes[0][1] = new PE_P5("P5");
		pes[0][1]->clock(clock);
		pes[0][1]->set_xy(1, 0);

		pes[1][1] = new PE_IO("PI/PO");
		pes[1][1]->clock(clock);
		pes[1][1]->set_xy(1, 1);

		pes[2][1] = new PE_P7("P7");
		pes[2][1]->clock(clock);
		pes[2][1]->set_xy(1, 2);

		pes[0][2] = new PE_P6("P6");
		pes[0][2]->clock(clock);
		pes[0][2]->set_xy(2, 0);

		pes[1][2] = new PE_P2("P2");
		pes[1][2]->clock(clock);
		pes[1][2]->set_xy(2, 1);

		pes[2][2] = new PE_P4("P4");
		pes[2][2]->clock(clock);
		pes[2][2]->set_xy(2, 2);
		
	}

	void create_network()
	{
		for (int i = 0; i < M; ++i)      //*create routers for the new architecture
		{

			for (int j = 0; j < N; ++j)
			{
			char name[100];
			sprintf(name, "router(%d,%d)", i,j);

			// create router
			routers[j][i] = new router(name);
			routers[j][i]->set_xy(i,j);
			routers[j][i]->clock(clock);

			if(j!=0)	// connect router to north routers
			{
				routers[j][i]->port_out[router::NORTH](
					router_to_router_north[j-1][i]);
				routers[j][i]->port_in[router::NORTH](
					router_to_router_south[j-1][i]);
			}
			else	//or make a loop
			{
				routers[j][i]->port_out[router::NORTH](
					terminal_loop_north[i]);
				routers[j][i]->port_in[router::NORTH](
					terminal_loop_north[i]);
			}
			if(j !=N-1)	// connect router to south routers
			{
				routers[j][i]->port_out[router::SOUTH](
					router_to_router_south[j][i]);
				routers[j][i]->port_in[router::SOUTH](
					router_to_router_north[j][i]);
			}
			else	// or make a loop
			{
				routers[j][i]->port_out[router::SOUTH](
					terminal_loop_south[i]);
				routers[j][i]->port_in[router::SOUTH](
					terminal_loop_south[i]);
			}

			// connect router to west routers
			if (i != 0)
			{
				routers[j][i]->port_out[router::WEST](
					router_to_router_west[j][i-1]);
				routers[j][i]->port_in[router::WEST](
					router_to_router_east[j][i-1]);
			}
			else // or make a loop
			{
				routers[j][i]->port_out[router::WEST](
					terminal_loop_west[j]);
				routers[j][i]->port_in[router::WEST](
					terminal_loop_west[j]);
			}

			if (i != M-1) // connect router to east routers
			{
				routers[j][i]->port_out[router::EAST](
					router_to_router_east[j][i]);
				routers[j][i]->port_in[router::EAST](
					router_to_router_west[j][i]);
			}
			else // or make a loop
			{
				routers[j][i]->port_out[router::EAST](
					terminal_loop_east[j]);
				routers[j][i]->port_in[router::EAST](
					terminal_loop_east[j]);
			}

			// connect router to PE
			routers[j][i]->port_out[router::PE](router_to_pe[j][i]);
			routers[j][i]->port_in[router::PE](pe_to_router[j][i]);
			pes[j][i]->data_in(router_to_pe[j][i]);
			pes[j][i]->data_out(pe_to_router[j][i]);

			}
		}
	}

}; // top

int sc_main(int argc , char *argv[])
{
	srand(0);

	top top_module("top");

	printf("cycle  0 ================================\n");
	sc_start(0, SC_NS);

	for(; i < 50; i++){
		
		printf("cycle %2d ================================\n", i);

		top_module.clock.write(1);
		sc_start(10, SC_NS);
		top_module.clock.write(0);
		sc_start(10, SC_NS);
	}

	extern string::size_type queue_p11r11; // call the value of queue size
	extern string::size_type queue_p01r01;
	extern string::size_type queue_p21r21;
	extern string::size_type queue_p00r00;
	extern string::size_type queue_p22r22;
	extern string::size_type queue_p10r10;
	extern string::size_type queue_p20r20;
	extern string::size_type queue_p12r12;
	extern string::size_type queue_p02r02;

	extern string::size_type queue_r00p00;
	extern string::size_type queue_r01p01;
	extern string::size_type queue_r02p02;
	extern string::size_type queue_r10p10;
	extern string::size_type queue_r11p11;
	extern string::size_type queue_r12p12;
	extern string::size_type queue_r20p20;
	extern string::size_type queue_r21p21;
	extern string::size_type queue_r22p22;

	extern string::size_type queue_r00_e;
	extern string::size_type queue_r00_s;
	extern string::size_type queue_r01_n;
	extern string::size_type queue_r01_s;
	extern string::size_type queue_r01_e;
    extern string::size_type queue_r02_n;
	extern string::size_type queue_r02_e;
	extern string::size_type queue_r10_w;
	extern string::size_type queue_r10_e; 
	extern string::size_type queue_r10_s;
	extern string::size_type queue_r11_e;
	extern string::size_type queue_r11_w;
	extern string::size_type queue_r11_n;
	extern string::size_type queue_r11_s;
	extern string::size_type queue_r12_w;
	extern string::size_type queue_r12_e;
	extern string::size_type queue_r12_n;
	extern string::size_type queue_r20_w;
	extern string::size_type queue_r20_s;
	extern string::size_type queue_r21_n;
	extern string::size_type queue_r21_s;
	extern string::size_type queue_r21_w;
	extern string::size_type queue_r22_w;
	extern string::size_type queue_r22_n;

	extern int round_start;
	extern int round_end;
	extern int PO_fire_time;


	cout << endl;
	
	cout << "Queue Size" << endl;
	printf("================================\n");
	printf("=============== For PE to Router =================\n");
	cout << "PEI/O(1, 1) to RouterI/O(1, 1) Queue Size is " << queue_p11r11 << endl;  // output queue size
	cout << "PE1(0, 1) to Router1(0, 1) Queue Size is " << queue_p01r01 << endl;
	cout << "PE2(2, 1) to Router2(2, 1) Queue Size is " << queue_p21r21 << endl;
	cout << "PE3(0, 0) to Router3(0, 0) Queue Size is " << queue_p00r00 << endl;
	cout << "PE4(2, 2) to Router4(2, 2) Queue Size is " << queue_p22r22 << endl;
	cout << "PE5(1, 0) to Router5(1, 0) Queue Size is " << queue_p10r10 << endl;
	cout << "PE6(2, 0) to Router6(2, 0) Queue Size is " << queue_p20r20 << endl;
	cout << "PE7(1, 2) to Router7(1, 2) Queue Size is " << queue_p12r12 << endl;
	cout << "PE8(0, 2) to Router8(0, 2) Queue Size is " << queue_p02r02 << endl;

	cout << endl;
	printf("=============== For Router to PE =================\n");
	cout << "Router(0,0) to PE(0,0) Queue Size is " << queue_r00p00 << endl;
	cout << "Router(0,1) to PE(0,1) Queue Size is " << queue_r01p01 << endl;
	cout << "Router(0,2) to PE(0,2) Queue Size is " << queue_r02p02 << endl;
	cout << "Router(1,0) to PE(1,0) Queue Size is " << queue_r10p10 << endl;
	cout << "Router(1,1) to PE(1,1) Queue Size is " << queue_r11p11 << endl;
	cout << "Router(1,2) to PE(1,2) Queue Size is " << queue_r12p12 << endl;
	cout << "Router(2,0) to PE(2,0) Queue Size is " << queue_r20p20 << endl;
	cout << "Router(2,1) to PE(2,1) Queue Size is " << queue_r21p21 << endl;
	cout << "Router(2,2) to PE(2,2) Queue Size is " << queue_r22p22 << endl;

	cout << endl;
	printf("=============== For Router to Router =================\n");
	cout << endl;
	printf("=============== For Router(0,0) =================\n");
	cout << "Router(0,0) to North Router(1,0) " << queue_r00_e << endl;
	cout << "Router(0,0) to North Router(0,1) " << queue_r00_s << endl;
    cout << endl;
	printf("=============== For Router(0,1) =================\n");
	cout << "Router(0,1) to North Router(0,0) " << queue_r01_n << endl;
	cout << "Router(0,1) to South Router(0,2) " << queue_r01_s << endl;
	cout << "Router(0,1) to East Router(1,1) " << queue_r01_e << endl;
	cout << endl;
	printf("=============== For Router(0,2) =================\n");
	cout << "Router(0,2) to North Router(0,1) " << queue_r02_n << endl;
    cout << "Router(0,2) to North Router(1,2) " << queue_r02_e << endl;
	cout << endl;
	printf("=============== For Router(1,0) =================\n");
	cout << "Router(1,0) to West Router(0,0) " << queue_r10_w << endl;
	cout << "Router(1,0) to East Router(2,0) " << queue_r10_e << endl;
	cout << "Router(1,0) to East Router(1,1) " << queue_r10_s << endl;
	cout << endl;
	printf("=============== For Router(1,1) =================\n");
	cout << "Router(1,1) to East Router(2,1) " << queue_r11_e << endl;
	cout << "Router(1,1) to West Router(0,1) " << queue_r11_w << endl;
	cout << "Router(1,1) to North Router(1,1) " << queue_r11_n << endl;
	cout << "Router(1,1) to South Router(1,2) " << queue_r11_s << endl;
	cout << endl;
	printf("=============== For Router(1,2) =================\n");
	cout << "Router(1,2) to West Router(0,2) " << queue_r12_w << endl;
	cout << "Router(1,2) to East Router(2,2) " << queue_r12_e << endl;
	cout << "Router(1,2) to North Router(1,1) " << queue_r12_n << endl;
	cout << endl;
	printf("=============== For Router(2,0) =================\n");
	cout << "Router(2,0) to West Router(1,0) " << queue_r20_w << endl;
	cout << "Router(2,0) to South Router(2,1) " << queue_r20_s << endl;
	cout << endl;
	printf("=============== For Router(2,1) =================\n");
	cout << "Router(2,1) to North Router(2,0) " << queue_r21_n << endl;
	cout << "Router(2,1) to South Router(2,2) " << queue_r21_s << endl;
    cout << "Router(2,1) to West Router(1,1) " << queue_r21_w << endl;
	cout << endl;
	printf("=============== For Router(2,2) =================\n");
	cout << "Router(2,2) to West Router(1,2) " << queue_r22_w << endl;
	cout << "Router(2,2) to North Router(2,1) " << queue_r22_n << endl;


	//cout << "Maximum throughput is " << (float) (round_end-round_start)/PO_fire_time << endl;


	system("pause");
	return 0;
}
