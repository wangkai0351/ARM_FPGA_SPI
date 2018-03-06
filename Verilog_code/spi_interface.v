/***********************************************************************
　　   ****************** name：SPI_Slaver_Driver **************
***********************************************************************/

//spi四种模式SPI的相位(CPHA)和极性(CPOL)分别可以为0或1，对应的4种组合构成了SPI的4种模式(mode)

//Mode 0 CPOL=0, CPHA=0 
//Mode 1 CPOL=0, CPHA=1
//Mode 2 CPOL=1, CPHA=0 
//Mode 3 CPOL=1, CPHA=1

//时钟极性CPOL: 即SPI空闲时，时钟信号SCLK的电平（1:空闲时高电平; 0:空闲时低电平）
//时钟相位CPHA: 即SPI在SCLK第几个边沿开始采样（0:第一个边沿开始; 1:第二个边沿开始）

//use SPI 3 mode,CHOL = 1 (空闲电平为高电平),CHAL = 1 （下降沿采样，上升沿输出）
module spi
(            
	input clk,                                     //用fpga自己的25Mhz时钟
                     //"clk" needs to be faster than the SPI bus. Saxo-L has a default clock of 24MHz, which works fine here.
	input wire rst_n,

	output reg CS_N,                     //片选
	output reg SCK,                      //时钟
	output reg MOSI,                     //master output slave input
	input wire	MISO,                	//master input slave output
	
	output reg [7:0] byte_cnt,				//for test
	output reg [7:0]	txd_data,			//for test
  
//	output reg  [7:0] rxd_data,			//for test
//	output wire 		rxd_flag,			//for test
//	output wire 		rxd_flag_dly,		//for test
//	output wire 		rxd_flag_dly2,		//for test
//	output reg	[2:0] rxd_state,      	//for test
	output reg	[2:0] txd_state       	//for test
	
//	output wire sck_n,						//for test
//	output wire sck_p,						//for test
  
//	output reg wr_en,
//	output reg [15:0] data,
//	output	reg	[9:0]	address	  
);


parameter CS_HI = 10'd256;
parameter SCK_START_DLY = 10'd112;
parameter SCK_LO = 4'd3;
parameter SCK_PERIOD = 4'd7;
//parameter SEND_INTERVAL = 9'd50;
parameter BYTE_INTERVAL = 9'd1;    // 200 * clk

parameter BYTE_SEND_NUM = 9'd18;

reg start;
reg send_start;
reg send_over;
reg byte_interval;


reg [9:0] cs_hi_cnt;
reg [9:0] sck_start_cnt;
reg [3:0] sck_cnt;
//reg [7:0] byte_cnt;
reg [3:0] bit_cnt;
reg [7:0] interval_cnt;



/*******************CS_N************************/
always@(posedge clk or negedge rst_n)
begin 
	if(!rst_n)
		CS_N <= 1'b1;
	else begin
		if(cs_hi_cnt == CS_HI)
			CS_N <= 0;
		else if(send_over)
			CS_N <= 1'b1;
		else
			CS_N <= CS_N;
	end
end

always@(posedge clk or negedge rst_n)
begin 
	if(!rst_n)
		cs_hi_cnt <= 0;
	else if(CS_N)
		cs_hi_cnt <= cs_hi_cnt + 1'b1;
	else
		cs_hi_cnt <= 0;
end
/************************************************/




always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		start <= 0;

	else if(sck_start_cnt == SCK_START_DLY)
		start <= 1'b1;
	else if( send_over )
		start <= 0;
end

		
always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		sck_start_cnt <= 0;
	
	else if(!CS_N && !start ) 
		sck_start_cnt <= sck_start_cnt + 1'b1;
	else
			sck_start_cnt <= 0;
end

always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		send_start <= 0;
		
	else if(sck_start_cnt == SCK_START_DLY)
		send_start <= 1'b1;
	else if(interval_cnt == BYTE_INTERVAL && byte_cnt < BYTE_SEND_NUM)
		send_start <= 1'b1;
	else if(bit_cnt == 7 && sck_cnt == SCK_PERIOD)
		send_start <= 0;
end

always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		SCK <= 1'b1;
	else begin
		if(send_start) begin
			if(sck_cnt < SCK_LO)
				SCK <= 0;
			else if(sck_cnt < SCK_PERIOD)
				SCK <= 1'b1;
			else if(bit_cnt < 7) 
				SCK <= 0;
		end
	end
end		
				
always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		sck_cnt <= 0;
		
	else if(send_start) begin
		if(sck_cnt < SCK_PERIOD)
			sck_cnt <= sck_cnt + 1'b1;
		else
			sck_cnt <= 1'b1;
	end
	else
		sck_cnt <= 0;
		
end


always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		bit_cnt <= 0;
	else begin
		if(sck_cnt == SCK_PERIOD)
			bit_cnt <= bit_cnt + 1'b1;
		else if(interval_cnt == BYTE_INTERVAL)
			bit_cnt <= 0;
	end
end

always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		byte_cnt <= 0;
		
	else begin
		if(bit_cnt == 7 && sck_cnt == SCK_PERIOD) 
			byte_cnt <= byte_cnt + 1'b1;
		else if(send_over)
			byte_cnt <= 0;
	end
end

always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		byte_interval <= 0;
		
	else begin
		if(bit_cnt == 8 && interval_cnt < BYTE_INTERVAL)
			byte_interval <= 1'b1;
		else
			byte_interval <= 0;
	end
end

always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		interval_cnt <= 0;
		
	else begin
		if(byte_interval)
			interval_cnt <= interval_cnt + 1'b1;
		else
			interval_cnt <= 0;
	end
end
			
always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		send_over <= 0;
		
	else begin
		if(byte_cnt == BYTE_SEND_NUM && interval_cnt == BYTE_INTERVAL)
			send_over <= 1'b1;
		else if(CS_N)
			send_over <= 0;
	end
end			
		



/*********************** out MOSI***********************/
always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		txd_data <= 0;
	
	else if(!send_start) 
	begin
		case (byte_cnt)
			8'd0 : txd_data <= 8'hAA;
			8'd1 : txd_data <= 8'h55;
			8'd2 : txd_data <= 8'hA4;
			8'd3 : txd_data <= 8'h00;
			8'd4 : txd_data <= 8'h02;
			
			8'd5 : txd_data <= 8'hAA;
			8'd6 : txd_data <= 8'h55;
			8'd7 : txd_data <= 8'hA5;
			8'd8 : txd_data <= 8'h01;
			8'd9 : txd_data <= 8'h02;
			
			8'd10 : txd_data <= 8'hAA;
			8'd11 : txd_data <= 8'h55;
			8'd12 : txd_data <= 8'hA6;
			8'd13 : txd_data <= 8'h01;
			8'd14 : txd_data <= 8'h02;
			8'd15 : txd_data <= 8'h03;
			8'd16 : txd_data <= 8'h04;
			8'd17 : txd_data <= 8'h05;
		endcase
	end
end

always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		MOSI = 0;
		
	else if(send_start) begin
		if(sck_cnt == 0)
			MOSI <= txd_data[7];
		else if(sck_cnt == SCK_PERIOD) begin
			case(bit_cnt)
				8'd0 : MOSI <= txd_data[6];
				8'd1 : MOSI <= txd_data[5];
				8'd2 : MOSI <= txd_data[4];
				8'd3 : MOSI <= txd_data[3];
				8'd4 : MOSI <= txd_data[2];
				8'd5 : MOSI <= txd_data[1];
				8'd6 : MOSI <= txd_data[0];
			endcase
		end
		else
			MOSI <= MOSI;
	end
end
		



////-------------------------capture the sck-----------------------------	
////直接用SCK信号当从机的clock信号也行，捕获sck上下沿是一种优化手段	  
//reg	sck_r0,sck_r1;
////wire sck_n,sck_p;
//always@(posedge clk or negedge rst_n)
//begin
//	if(!rst_n)
//		begin
//			sck_r0 <= 1'b1;   //sck of the idle state is high 
//			sck_r1 <= 1'b1;
//		end
//	else
//		begin
//			sck_r0 <= SCK;
//			sck_r1 <= sck_r0;
//		end
//end
//
//
//assign sck_n = (~sck_r0 & sck_r1)? 1'b1:1'b0;   //capture the sck negedge
//assign sck_p = (~sck_r1 & sck_r0)? 1'b1:1'b0;   //capture the sck posedge

////-----------------------spi_slaver read data-------------------------------
//reg  rxd_flag_h;
////	 [2:0] rxd_state;
//always@(posedge clk or negedge rst_n)
//begin
//	if(!rst_n)
//		begin
//			rxd_data <= 1'b0;
//			rxd_flag_h <= 1'b0;
//			rxd_state <= 1'b0;
//		end
//	else if(CS_N)
//		begin
//			rxd_data <= 1'b0;
//			rxd_flag_h <= 1'b0;
//			rxd_state <= 1'b0;		
//		end
//	else if(sck_p && !CS_N)               //sck上升沿输出，片选信号低电平有效 
//		begin
//			case(rxd_state)
//				3'd0:begin
//						rxd_data[7] <= MOSI;    //master output slave input 从高位开始
//						rxd_flag_h <= 1'b0;     //reset rxd_flag
//						rxd_state <= 3'd1;
//					  end
//				3'd1:begin
//						rxd_data[6] <= MOSI;
//						rxd_state <= 3'd2;
//					  end
//				3'd2:begin
//						rxd_data[5] <= MOSI;
//						rxd_state <= 3'd3;
//					  end
//				3'd3:begin
//						rxd_data[4] <= MOSI;
//						rxd_state <= 3'd4;
//					  end
//				3'd4:begin
//						rxd_data[3] <= MOSI;
//						rxd_state <= 3'd5;
//					  end
//				3'd5:begin
//						rxd_data[2] <= MOSI;
//						rxd_state <= 3'd6;
//					  end
//				3'd6:begin
//						rxd_data[1] <= MOSI;
//						rxd_state <= 3'd7;
//					  end
//				3'd7:begin
//						rxd_data[0] <= MOSI;
//						rxd_flag_h <= 1'b1;  //set rxd_flag
//						rxd_state <= 3'd0;
//					  end
//				default: ;
//			endcase
//		end
//end
//
////--------------------capture spi_flag posedge--------------------------------
//reg	rxd_flag_h0,rxd_flag_h1,rxd_flag_h2;
//always@(posedge clk or negedge rst_n)
//begin
//	if(!rst_n)
//		begin
//			rxd_flag_h0 <= 1'b0;
//			rxd_flag_h1 <= 1'b0;
//		end
//	else
//		begin
//			rxd_flag_h0 <= rxd_flag_h;
//			rxd_flag_h1 <= rxd_flag_h0;
//			rxd_flag_h2 <= rxd_flag_h1;
//		end
//end
//
//
//assign rxd_flag = (rxd_flag_h & ~rxd_flag_h0)? 1'b1:1'b0;
//assign rxd_flag_dly = (rxd_flag_h0 & ~rxd_flag_h1)? 1'b1:1'b0;
//assign rxd_flag_dly2 = (rxd_flag_h1 & ~rxd_flag_h2)? 1'b1:1'b0;


////---------------------spi_slaver send data---------------------------
////	 [2:0] txd_state;
//always@(posedge clk or negedge rst_n)
//begin
//	if(!rst_n)
//		begin
//			txd_state <= 1'b0;
//		end
//	else if(CS_N)
//		begin
//			txd_state <= 1'b0;
//		end
//	else if(sck_n && !CS_N)           //sck下降沿采样，片选信号低电平有效 
//		begin
//			case(txd_state)
//				3'd0:begin
//						MISO <= txd_data[7];
//						txd_state <= 3'd1;
//					  end
//				3'd1:begin
//						MISO <= txd_data[6];
//						txd_state <= 3'd2;
//					  end
//				3'd2:begin
//						MISO <= txd_data[5];
//						txd_state <= 3'd3;
//					  end
//				3'd3:begin
//						MISO <= txd_data[4];
//						txd_state <= 3'd4;
//					  end
//				3'd4:begin
//						MISO <= txd_data[3];
//						txd_state <= 3'd5;
//					  end
//				3'd5:begin
//						MISO <= txd_data[2];
//						txd_state <= 3'd6;
//					  end
//				3'd6:begin
//						MISO <= txd_data[1];
//						txd_state <= 3'd7;
//					  end
//				3'd7:begin
//						MISO <= txd_data[0];
//						txd_state <= 3'd0;
//					  end
//				default: ;
//			endcase
//		end
//end



////---------------------resolve spi "rxd_data"---------------------------
//reg	[3:0]  fsm_spi_cs;
//reg	[3:0]  fsm_spi_ns;
//
//reg	[15:0] data_cnt;
//reg	[15:0] data_len;
//reg	len_Type_recved;
//reg	addr_Type_recved;
//reg	data_Type_recved;
//reg 	data_start;
//
//parameter IDLE            	= 4'd1;
//parameter RECV_PRENUM1    	= 4'd2;
//parameter RECV_PRENUM2    	= 4'd3;
//parameter RECV_TYPE       	= 4'd4;
//parameter RECV_LENGTH1		= 4'd5;
//parameter RECV_LENGTH2		= 4'd6;
//parameter RECV_ADDRESS1		= 4'd7;
//parameter RECV_ADDRESS2		= 4'd8;
//parameter RECV_DATA1			= 4'd9;			//Double Word!
//parameter RECV_DATA2			= 4'd10;
//
//always@(posedge clk or negedge rst_n)
//begin
//	if(!rst_n)
//		fsm_spi_cs <= IDLE;
//	else
//		fsm_spi_cs <= fsm_spi_ns;
//end
//
//
//always@(posedge clk)
//begin
//	if(!rst_n)
//		fsm_spi_ns = IDLE;
//   else if(CS_N)
//		fsm_spi_ns = IDLE;
//	
//	else if(rxd_flag)	begin		//spi_Byte_recv done
//		case(fsm_spi_cs)
//			IDLE: begin
//				if(rxd_data == 8'hAA)
//					fsm_spi_ns = RECV_PRENUM1;
//				else
//					fsm_spi_ns = IDLE;
//			end
//			
//			RECV_PRENUM1: begin
//				if(rxd_data == 8'h55)
//					fsm_spi_ns = RECV_PRENUM2;
//				else
//					fsm_spi_ns = IDLE;
//			end
//				
//			RECV_PRENUM2: 
//				fsm_spi_ns = RECV_TYPE;
//						
//			RECV_TYPE: begin
//				if( len_Type_recved && (!addr_Type_recved) )
//					fsm_spi_ns = RECV_LENGTH1;
//				else if(addr_Type_recved && (!data_Type_recved) )
//					fsm_spi_ns = RECV_ADDRESS1;
//				else if(data_Type_recved)
//					fsm_spi_ns = RECV_DATA1;
//			end
//			
//			RECV_LENGTH1: begin
//				if(!len_Type_recved)
//					fsm_spi_ns = IDLE;
//				else
//					fsm_spi_ns = RECV_LENGTH2;
//			end
//			
//			RECV_LENGTH2: begin
//				if(rxd_data == 8'hAA)
//					fsm_spi_ns = RECV_PRENUM1;
//				else
//					fsm_spi_ns = IDLE;
//			end
//			
//			RECV_ADDRESS1: begin
//				if(!len_Type_recved)
//					fsm_spi_ns = IDLE;
//				else
//					fsm_spi_ns = RECV_ADDRESS2;
//			end
//				
//			RECV_ADDRESS2: begin
//				if(rxd_data == 8'hAA)
//					fsm_spi_ns = RECV_PRENUM1;
//				else
//					fsm_spi_ns = IDLE;
//			end
//			
//			RECV_DATA1: begin
//				if(!addr_Type_recved)
//					fsm_spi_ns = IDLE;
//				else begin
//					fsm_spi_ns = RECV_DATA2;
//				end
//			end
//
//			RECV_DATA2: begin
//				if(data_cnt < data_len)
//					fsm_spi_ns = RECV_DATA1;
//				else
//					fsm_spi_ns = IDLE;
//			end
//			
//			default: 
//				fsm_spi_ns = IDLE;
//
//		endcase
//	end
//end
//
//
//always@(posedge clk or negedge rst_n)
//begin
//	if(!rst_n) begin
//		data_cnt <= 0;
//		data_len <= 0;
//		
//		len_Type_recved <= 0;
//		addr_Type_recved<= 0;
//		data_Type_recved <= 0;
//		data_start <= 0;
//		
//		address <= 0;
//		data <= 0;
//	end
//	
//	else begin
//		case(fsm_spi_ns)
//			IDLE: begin
//				data_cnt <= 0;
//				data_len <= 0;
//
//				len_Type_recved <= 0;
//				addr_Type_recved<= 0;
//				data_Type_recved <= 0;
//				data_start <= 0;
//				
//				address <= 0;
//				data <= 0;
//			end
//			
//			RECV_PRENUM1: ;
//			RECV_PRENUM2: ;
//			
//			RECV_TYPE: begin
//				if(rxd_flag_dly) begin
//					if(rxd_data == 8'hA4)
//						len_Type_recved <= 1'd1;
//					else if(rxd_data == 8'hA5)
//						addr_Type_recved <= 1'd1;
//					else if(rxd_data == 8'hA6)
//						data_Type_recved <= 1'd1;
//				end
//			end
//			
//			RECV_LENGTH1: 
//				if(rxd_flag_dly)
//					data_len[15:8] <= rxd_data;
//			
//			RECV_LENGTH2: 
//				if(rxd_flag_dly)
//					data_len[7:0] <= rxd_data;
//			
//			RECV_ADDRESS1:
//				if(rxd_flag_dly)
//					address[9:8] <= rxd_data;
//				
//			RECV_ADDRESS2:
//				if(rxd_flag_dly)
//					address[7:0] <= rxd_data[1:0];
//			
//			RECV_DATA1: 
//				if(rxd_flag_dly)
//					data[15:8] <= rxd_data;
//				
//			RECV_DATA2: begin
//				wr_en <= rxd_flag_dly;
//				if(rxd_flag_dly) begin
//					data_start <= 1'd1;
//					data_cnt <= data_cnt + 8'd1;
//					data[7:0] <= rxd_data;
//					if(data_start)
//						address <= address + 1'd1;
//				end
//			end
//			
//			default: ;
//			
//		endcase
//	end		
//end


endmodule








