#include "router.h"

using std::string;
using std::vector;


string::size_type queue_r00p00;  //define queue size between routers
string::size_type queue_r01p01;
string::size_type queue_r02p02;
string::size_type queue_r10p10;
string::size_type queue_r11p11;
string::size_type queue_r12p12;
string::size_type queue_r20p20;
string::size_type queue_r21p21;
string::size_type queue_r22p22;

string::size_type queue_r00_e;
string::size_type queue_r00_s;
string::size_type queue_r01_n;
string::size_type queue_r01_s;
string::size_type queue_r01_e;
string::size_type queue_r02_n;
string::size_type queue_r02_e;
string::size_type queue_r10_w;
string::size_type queue_r10_e;
string::size_type queue_r10_s;
string::size_type queue_r11_e;
string::size_type queue_r11_w;
string::size_type queue_r11_n;
string::size_type queue_r11_s;
string::size_type queue_r12_w;
string::size_type queue_r12_e;
string::size_type queue_r12_n;
string::size_type queue_r20_w;
string::size_type queue_r20_s;
string::size_type queue_r21_n;
string::size_type queue_r21_s;
string::size_type queue_r21_w;
string::size_type queue_r22_n;
string::size_type queue_r22_w;


std::ostream &operator<<(std::ostream &o, const packet &p)
{
	char buf[100];
	sprintf(buf, "(%d,%d)->(%d,%d)",
		p.src_x, p.src_y, p.dest_x, p.dest_y);
	return o << buf << ", " << p.token.Rp<<p.token.Ip;
}

void sc_trace(sc_trace_file *tf, const packet &p, const std::string &name)
{
	sc_trace(tf, p.src_x, name+".src.x");
	sc_trace(tf, p.src_y, name+".src.y");
	sc_trace(tf, p.dest_x, name+".dest.x");
	sc_trace(tf, p.dest_y, name+".dest.y");
	sc_trace(tf, p.token.Rp, name+".token.Rp");
	sc_trace(tf, p.token.Ip, name + ".token.Ip");
}

void router::main()
{
	assert((x_ != -1) && (y_ != -1)); // to identify PE

	for (int iport = 0; iport < PORTS; ++iport)
		read_packet(iport);

	for (int iport = 0; iport < PORTS; ++iport)
		write_packet(iport);
}

void router::set_xy(int x, int y)
{
	assert((x_ == -1) && (y_ == -1)); // set once only
	assert((x != -1) && (y != -1)); // must use a legal location

	x_ = x;
	y_ = y;
}

void router::read_packet(int iport)
{
	assert(iport < PORTS);

	packet p = port_in[iport].read();

	if ((p.src_x == -1) && (p.src_y == -1))
		return; // empty packet

	route_packet_xy(p);
}

void router::write_packet(int iport)
{
	assert(iport < PORTS);

	if (out_queue_[iport].empty())
	{
		port_out[iport].write(packet()); // write an empty packet
	}
	else
	{
		port_out[iport].write(out_queue_[iport].front());
		out_queue_[iport].pop_front();
	}
}

void router::route_packet_xy(packet p)
{
	if ((p.dest_x == -1) || (p.dest_y == -1))
	{
		printf("router (%d,%d): drop packet with invalid destination"
			" (%d,%d)->(%d,%d)\n",
			x_, y_,
			p.src_x, p.src_y, p.dest_x, p.dest_y);
		return;
	}

	//*modify this part according to the XY routing algorithm
	if ((p.dest_x == x_) && (p.dest_y == y_)) // *to PE
	{
		out_queue_[PE].push_back(p);
		if (x_ == 0 && y_ == 0 && queue_r00p00 < out_queue_[PE].size())   // calculate queue size between routers
			queue_r00p00 = out_queue_[PE].size();
		if (x_ == 0 && y_ == 1 && queue_r01p01 < out_queue_[PE].size())
			queue_r01p01 = out_queue_[PE].size();
		if (x_ == 0 && y_ == 2 && queue_r02p02 < out_queue_[PE].size())
			queue_r02p02 = out_queue_[PE].size();
		if (x_ == 1 && y_ == 0 && queue_r10p10 < out_queue_[PE].size())
			queue_r10p10 = out_queue_[PE].size();
		if (x_ == 1 && y_ == 1 && queue_r11p11 < out_queue_[PE].size())
			queue_r11p11 = out_queue_[PE].size();
		if (x_ == 1 && y_ == 2 && queue_r12p12 < out_queue_[PE].size())
			queue_r12p12 = out_queue_[PE].size();
		if (x_ == 2 && y_ == 0 && queue_r20p20 < out_queue_[PE].size())
			queue_r20p20 = out_queue_[PE].size();
		if (x_ == 2 && y_ == 1 && queue_r21p21 < out_queue_[PE].size())
			queue_r21p21 = out_queue_[PE].size();
		if (x_ == 2 && y_ == 2 && queue_r22p22 < out_queue_[PE].size())
			queue_r22p22 = out_queue_[PE].size();
		
	}

	if (p.dest_y == y_) //* to west or east
	{
		if (p.dest_x < x_)   //*to west
		{
			out_queue_[WEST].push_back(p);
			if (x_ == 1 && y_ == 0 && queue_r10_w < out_queue_[WEST].size())
				queue_r10_w = out_queue_[WEST].size();
			if (x_ == 1 && y_ == 1 && queue_r11_w < out_queue_[WEST].size())
				queue_r11_w = out_queue_[WEST].size();
			if (x_ == 1 && y_ == 2 && queue_r12_w < out_queue_[WEST].size())
				queue_r12_w = out_queue_[WEST].size();
			if (x_ == 2 && y_ == 0 && queue_r20_w < out_queue_[WEST].size())
				queue_r20_w = out_queue_[WEST].size();
			if (x_ == 2 && y_ == 1 && queue_r21_w < out_queue_[WEST].size())
				queue_r21_w = out_queue_[WEST].size();
			if (x_ == 2 && y_ == 2 && queue_r22_w < out_queue_[WEST].size())
				queue_r22_w = out_queue_[WEST].size();
		}
		if (p.dest_x > x_)   //*to east
		{
			out_queue_[EAST].push_back(p);
			if (x_ == 0 && y_ == 0 && queue_r00_e < out_queue_[EAST].size())
				queue_r00_e = out_queue_[EAST].size();
			if (x_ == 0 && y_ == 1 && queue_r01_e < out_queue_[EAST].size())
				queue_r01_e = out_queue_[EAST].size();
			if (x_ == 0 && y_ == 2 && queue_r02_e < out_queue_[EAST].size())
				queue_r02_e = out_queue_[EAST].size();
			if (x_ == 1 && y_ == 0 && queue_r10_e < out_queue_[EAST].size())
				queue_r10_e = out_queue_[EAST].size();
			if (x_ == 1 && y_ == 1 && queue_r11_e < out_queue_[EAST].size())
				queue_r11_e = out_queue_[EAST].size();
			if (x_ == 1 && y_ == 2 && queue_r12_e < out_queue_[EAST].size())
				queue_r12_e = out_queue_[EAST].size();

		}
	}

	if (p.dest_y != y_)   //*to north or south
	{
		if (p.dest_y < y_)  //*to north
		{
			out_queue_[NORTH].push_back(p);
			if (x_ == 0 && y_ == 1 && queue_r01_n < out_queue_[NORTH].size())
				queue_r01_n = out_queue_[NORTH].size();
			if (x_ == 0 && y_ == 2 && queue_r02_n < out_queue_[NORTH].size())
				queue_r02_n = out_queue_[NORTH].size();
			if (x_ == 1 && y_ == 1 && queue_r11_n < out_queue_[NORTH].size())
				queue_r11_n = out_queue_[NORTH].size();
			if (x_ == 1 && y_ == 2 && queue_r12_n < out_queue_[NORTH].size())
				queue_r12_n = out_queue_[NORTH].size();
			if (x_ == 2 && y_ == 1 && queue_r21_n < out_queue_[NORTH].size())
				queue_r21_n = out_queue_[NORTH].size();
			if (x_ == 2 && y_ == 2 && queue_r22_n < out_queue_[NORTH].size())
				queue_r22_n = out_queue_[NORTH].size();
		}
		if (p.dest_y > y_)   //*to south
		{
			out_queue_[SOUTH].push_back(p);
			if (x_ == 0 && y_ == 0 && queue_r00_s < out_queue_[SOUTH].size())
				queue_r00_s = out_queue_[SOUTH].size();
			if (x_ == 0 && y_ == 1 && queue_r01_s < out_queue_[SOUTH].size())
				queue_r01_s = out_queue_[SOUTH].size();
			if (x_ == 1 && y_ == 0 && queue_r10_s < out_queue_[SOUTH].size())
				queue_r10_s = out_queue_[SOUTH].size();
			if (x_ == 1 && y_ == 1 && queue_r11_s < out_queue_[SOUTH].size())
				queue_r11_s = out_queue_[SOUTH].size();
			if (x_ == 2 && y_ == 0 && queue_r20_s < out_queue_[SOUTH].size())
				queue_r20_s = out_queue_[SOUTH].size();
			if (x_ == 2 && y_ == 1 && queue_r21_s < out_queue_[SOUTH].size())
				queue_r21_s = out_queue_[SOUTH].size();
		}
	}

}


