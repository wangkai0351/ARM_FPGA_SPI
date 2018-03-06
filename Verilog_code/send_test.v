/*
	module: send_test.v

*/
/*************************************************************
File name   :mii_send.v
Function    :control the package send to PHY device
Author      :tongqing
Version     :V1.0
**************************************************************/	
module send_test(
//		input wire i_clk,
//		input wire i_rst_n, 
//		input wire i_SendIrq,
//		input wire [15:0] i_data,
//		input wire [9:0] i_length,
		output reg o_sendDn,
		output reg o_sendIdl,
//		output reg [3:0] o_send_data_1,
//		output reg o_Tx_dv_1,
//		output reg [9:0] o_send_data_1_addr		
	 input  wire         i_clk_125m,
	 input  wire        i_clk_250m,
    input  wire         i_rst_n,
    output wire         o_Tx_dv_1,
    output wire  [3:0]  o_send_data_1
);
reg sendIrq;
reg [31:0] sendIrq_num;
reg [4:0] fsm_cs;
reg [4:0] fsm_ns;
reg [7:0] dly_counter;
reg [3:0] pre_num_counter;
wire [31:0] CRC; 
reg sendEn_dly1;
reg sendIdl;
reg [7:0] ifg_num;
parameter IDLE                  = 5'b00000;
parameter WAIT_IFG              = 5'b00001;
parameter SEND_START            = 5'b00011;
parameter SEND_PRE_NUM          = 5'b00010;
parameter SEND_START_NUM        = 5'b00110;
parameter SEND_1DATA            = 5'b00111;
parameter SEND_2DATA            = 5'b00101;
parameter SEND_3DATA            = 5'b01101;
parameter SEND_4DATA            = 5'b01111;
parameter SEND_1CRC             = 5'b01110;
parameter SEND_2CRC             = 5'b01010;
parameter SEND_3CRC             = 5'b01011;
parameter SEND_4CRC             = 5'b01001;
parameter SEND_5CRC             = 5'b01000;
parameter SEND_6CRC             = 5'b11000;
parameter SEND_7CRC             = 5'b11001;
parameter SEND_8CRC             = 5'b11011;
parameter SEND_OVER             = 5'b11010;
parameter SEND_OVER_DLY         = 5'b11110;
reg [9:0] ram_rdNum;
reg [15:0] send_data;
reg [15:0] CRC_data;
wire [15:0] send_data_tmp[29:0];
assign send_data_tmp[0] = 16'hffff;
assign send_data_tmp[1] = 16'hffff;
assign send_data_tmp[2] = 16'hffff;
assign send_data_tmp[3] = 16'h0001;
assign send_data_tmp[4] = 16'h0203;
assign send_data_tmp[5] = 16'h0401;
assign send_data_tmp[6] = 16'h1437;
assign send_data_tmp[7] = 16'h1001;
assign send_data_tmp[8] = 16'hc0a8;
assign send_data_tmp[9] = 16'h0101;
assign send_data_tmp[10] = 16'h0;
assign send_data_tmp[11] = 16'h0;
assign send_data_tmp[12] = 16'h0;
assign send_data_tmp[13] = 16'h0;
assign send_data_tmp[14] = 16'h0;
assign send_data_tmp[15] = 16'h0;
assign send_data_tmp[16] = 16'h0;
assign send_data_tmp[17] = 16'h0;
assign send_data_tmp[18] = 16'h0;
assign send_data_tmp[19] = 16'h0;
assign send_data_tmp[20] = 16'h0;
assign send_data_tmp[21] = 16'h0;
assign send_data_tmp[22] = 16'h0;
assign send_data_tmp[23] = 16'h0;
assign send_data_tmp[24] = 16'h0;
assign send_data_tmp[25] = 16'h0;
assign send_data_tmp[26] = 16'h0;
assign send_data_tmp[27] = 16'h0;
assign send_data_tmp[28] = 16'h0;
assign send_data_tmp[29] = 16'h0;
reg [3:0]ram_data[0:143];
reg wren;
reg rden;
reg [7:0] address;
reg [7:0] NUM;
reg [3:0] o_send_data_3;
always@(posedge i_clk_125m)
begin
  if (wren)
  begin
  ram_data[address]<=o_send_data_3;
  end
  else
  ram_data[address]<=ram_data[address];
end
reg [3:0] send_data_1,send_data_2;
reg tx_en_1,tx_en_2;
reg [7:0] NUM_1,NUM_2;
assign o_Tx_dv_1 = tx_en_2;
assign o_send_data_1 = i_clk_125m?send_data_1:send_data_2;
always @(posedge i_clk_250m or negedge i_rst_n)
begin
	if(!i_rst_n)
	begin
	tx_en_1 <= 1'b0;
	send_data_1 <= 4'b0;
	NUM_1 <= 8'd0;
	end
  else if(rden&&NUM_1<8'h90)
  begin
  tx_en_1 <= 1'b1;
  send_data_1<=ram_data[NUM_1];
  NUM_1<=NUM_1+8'h02;
  end 
  else
  begin
  tx_en_1 <= 1'b0;
  send_data_1<=4'b0;
  NUM_1<=8'h0;
  end
end

always @(posedge i_clk_125m or negedge i_rst_n)
begin
	if(!i_rst_n)
	begin
	tx_en_2 <= 1'b0;
	send_data_2 <= 4'b0;
	NUM_2 <= 8'd1;
	end
  else if(rden && NUM_2 < 8'h90)
  begin
  tx_en_2 <= 1'b1;
  send_data_2<=ram_data[NUM_2];
  NUM_2<=NUM_2+8'h02;
  end 
  else
  begin
  tx_en_2 <= 1'b0;
  send_data_2<=4'b0;
  NUM_2<=8'h1;
  end
end
//always @(posedge i_clk_250m )
//begin
//  if(rden&&NUM<8'h90)
//  begin
//  o_Tx_dv_1 <= 1'b1;
//  o_send_data_1<=ram_data[NUM];
//  NUM<=NUM+8'h02;
//  end
//  else if(rden&&NUM==8'h90)
//  begin
//  o_Tx_dv_1 <= 1'b0 ;
//  o_send_data_1<=ram_data[NUM];
//  NUM<=NUM+8'h01;
//  end 
//  else
//  begin
//  o_send_data_1<=4'b0;
//  NUM<=8'h00;
//  o_Tx_dv_1 <= 1'b0;  
//  end
//end
always @(posedge i_clk_125m, negedge i_rst_n)
begin
	if(!i_rst_n)
		sendIrq_num <= sendIrq_num;
	else if(sendIrq_num <= 32'd125000)
		sendIrq_num <= sendIrq_num + 1'b1;
	else if(sendIrq_num> 32'd125000)
		sendIrq_num <= 32'd0;
	else 
		sendIrq_num <= sendIrq_num;
end

always @(posedge i_clk_125m,negedge i_rst_n)
begin
	if(!i_rst_n)
		sendIrq <= 1'b0;
	else if(sendIrq_num == 32'd125000)
		sendIrq <= 1'b1;
	else 
		sendIrq <= 1'b0;
end

always @(posedge i_clk_125m,negedge i_rst_n)
begin
	if(!i_rst_n)
		CRC_data <= 16'hffff;
	else if(fsm_ns == WAIT_IFG && ifg_num < 8'd30)
		CRC_data <= send_data_tmp[ifg_num];
//	else if(fsm_ns == WAIT_IFG &&ifg_num >8'd29)
//	   CRC_data <= 16'hffff;
	else 
//		CRC_data <= CRC_data;
      CRC_data <= 16'hffff;
end

always @(posedge i_clk_125m,negedge i_rst_n)
begin
	if(!i_rst_n)
		send_data <= 16'h0;
	else if(fsm_ns == SEND_4DATA && ram_rdNum >10'd29)
		send_data <= 16'h0;
	else if(fsm_ns == SEND_START_NUM)
		send_data <= send_data_tmp[ram_rdNum];
	else if(fsm_ns == SEND_4DATA && ram_rdNum < 10'd30)
		send_data <= send_data_tmp[ram_rdNum];
	else 
		send_data <= send_data;
end
always @ (posedge i_clk_125m,negedge i_rst_n)
begin
    if (!i_rst_n)
    begin       
        dly_counter <= 4'h0;
    end
    else if (fsm_cs ==  SEND_OVER)
    begin
        dly_counter <= dly_counter +1'b1;
    end
    else
    begin
        dly_counter <= 4'h0;
    end
end

/****************************************
 count the nibble number of preamble code
*****************************************/
always @ (posedge i_clk_125m,negedge i_rst_n)
begin
    if (!i_rst_n)
    begin       
       pre_num_counter <= 4'h0;
    end
    else if (fsm_cs ==  SEND_PRE_NUM)
    begin
        pre_num_counter <= pre_num_counter +1'b1;
    end
    else
    begin
        pre_num_counter <= 4'h0;
    end
end

/*************************************************
  1st always block, sequential state transition 
**************************************************/
always @ (posedge i_clk_125m , negedge i_rst_n)
begin
    if (!i_rst_n)
    begin
        fsm_cs <= IDLE;
    end
    else 
    begin
        fsm_cs  <= fsm_ns;
    end
end

/****************************************************
  2nd always block, combinational condition judgment
*****************************************************/
always @ (posedge i_clk_125m , negedge i_rst_n)
begin 	
    case (fsm_cs)
    IDLE:
	 begin
        if (sendIrq)
		  fsm_ns = WAIT_IFG;
		  else 
		  fsm_ns = IDLE;
	 end
	 WAIT_IFG:
    begin
        if(ifg_num >= 8'd32)
        fsm_ns = SEND_START;
        else 
        fsm_ns = WAIT_IFG;
    end	 
    SEND_START:
        fsm_ns = SEND_PRE_NUM;
    SEND_PRE_NUM:
    begin
        if(pre_num_counter >= 4'he)
        fsm_ns = SEND_START_NUM;
        else 
        fsm_ns = SEND_PRE_NUM;
    end
    SEND_START_NUM:
//			fsm_ns = SEND_START_NUM_DLY;
//	 SEND_START_NUM_DLY:
        fsm_ns = SEND_1DATA;
    SEND_1DATA:
        fsm_ns = SEND_2DATA;
    SEND_2DATA:
        fsm_ns = SEND_3DATA;
    SEND_3DATA:
        fsm_ns = SEND_4DATA;
    SEND_4DATA:
    begin
        if (ram_rdNum >= 10'd30)
        fsm_ns = SEND_1CRC;
        else 
        fsm_ns = SEND_1DATA;
    end
    SEND_1CRC:
        fsm_ns = SEND_2CRC;
    SEND_2CRC:
        fsm_ns = SEND_3CRC;
    SEND_3CRC:
        fsm_ns = SEND_4CRC;
    SEND_4CRC:
        fsm_ns = SEND_5CRC;
    SEND_5CRC:
        fsm_ns = SEND_6CRC;
    SEND_6CRC:
        fsm_ns = SEND_7CRC;
    SEND_7CRC:
        fsm_ns = SEND_8CRC;
    SEND_8CRC:
        fsm_ns = SEND_OVER;
    SEND_OVER:
	     if(dly_counter <= 8'd71)
	       begin	
           fsm_ns=SEND_OVER;			 		    
	       end
	       else
	       fsm_ns = SEND_OVER_DLY;
	 SEND_OVER_DLY:
	   fsm_ns=IDLE;
    default:
        fsm_ns = IDLE;
    endcase
end
/**************************************************
    3rd always block, the combinational FSM output
***************************************************/
always @ (posedge i_clk_125m, negedge i_rst_n)
begin
    if (!i_rst_n)
    begin
        o_send_data_3 <= 4'b0000;
//        o_Tx_dv_1 <= 1'b0;
        o_sendDn <= 1'b0;
        o_sendIdl <= 1'b1;
		  ifg_num <= 8'h0;
		  wren<=1'b0;
		  rden<=1'b0;
//		  address<=8'b0;
    end
    else 
    case (fsm_ns)
    IDLE:
    begin
        o_sendDn <= 1'b0;
//        o_Tx_dv_1 <= 1'b0; 
        o_sendIdl <= 1'b1;
        ram_rdNum <= 10'h0;
		  ifg_num <= 8'hff;
		  address<=8'hff;
		  wren<=1'b0;
    end
	 WAIT_IFG:
    begin
        o_sendDn <= 1'b0;
//        o_Tx_dv_1 <= 1'b0; 
//        o_sendIdl <= 1'b1;
        ram_rdNum <= 10'h0;
        ifg_num <= ifg_num + 1'b1;
    end	 
    SEND_START:
    begin
//        o_Tx_dv_1 <= 1'b0;  
        o_sendIdl <= 1'b0;
		  ram_rdNum <= 10'h0;
		  ifg_num <= 8'hff;
    end
    SEND_PRE_NUM:
    begin
        o_send_data_3<= 4'b0101;
//        o_Tx_dv_1 <= 1'b1;
//        o_sendIdl <= 1'b0;
        address<=address+1'b1;
		  wren<=1'b1;
    end                    
    SEND_START_NUM:
    begin
        o_send_data_3 <= 4'b1101;
        o_sendIdl <= 1'b0;
//		  o_Tx_dv_1 <= 1'b1;
		  address<=address+1'b1;
		  wren<=1'b1;
    end 
    SEND_1DATA:
    begin
//		  o_Tx_dv_1 <= 1'b1;
        o_send_data_3 <= send_data[11:8];
        o_sendIdl <= 1'b0;
		  address<=address+1'b1;
		  wren<=1'b1;
    end
    SEND_2DATA:
    begin
//		  o_Tx_dv_1 <= 1'b1;
        o_send_data_3 <= send_data[15:12];
        o_sendIdl <= 1'b0;
		  ram_rdNum <= ram_rdNum;
		  address<=address+1'b1;
		  wren<=1'b1;
    end                  
    SEND_3DATA:
    begin
//		  o_Tx_dv_1 <= 1'b1;
        o_send_data_3 <= send_data[3:0];
        o_sendIdl <= 1'b0;
		  ram_rdNum <= ram_rdNum + 1'b1;
		  address<=address+1'b1;
		  wren<=1'b1;
    end
    SEND_4DATA:
    begin
//        o_Tx_dv_1 <= 1'b1;
		  o_send_data_3 <= send_data[7:4];
        o_sendIdl <= 1'b0;
		  ram_rdNum <= ram_rdNum;
		  address<=address+1'b1;
		  wren<=1'b1;
    end
    SEND_1CRC:
    begin
        o_send_data_3 <= {CRC[28],CRC[29],CRC[30],CRC[31]};
        o_sendIdl <= 1'b0;
		  address<=address+1'b1;
		  wren<=1'b1;
    end
    SEND_2CRC:
    begin
        o_send_data_3 <= {CRC[24],CRC[25],CRC[26],CRC[27]};
        o_sendIdl <= 1'b0;
	     address<=address+1'b1;
		  wren<=1'b1;	  
    end
    SEND_3CRC:
    begin
        o_send_data_3 <= {CRC[20],CRC[21],CRC[22],CRC[23]};
        o_sendIdl <= 1'b0;
		  address<=address+1'b1;
		  wren<=1'b1;
    end
    SEND_4CRC:
    begin
        o_send_data_3 <= {CRC[16],CRC[17],CRC[18],CRC[19]};
        o_sendIdl <= 1'b0;
        address<=address+1'b1;
		  wren<=1'b1;		  
    end
    SEND_5CRC:
    begin
        o_send_data_3 <= {CRC[12],CRC[13],CRC[14],CRC[15]};
        o_sendIdl <= 1'b0;
		  address<=address+1'b1;
		  wren<=1'b1;
    end
    SEND_6CRC:
    begin
        o_send_data_3 <= {CRC[8],CRC[9],CRC[10],CRC[11]};
        o_sendIdl <= 1'b0;
		  address<=address+1'b1;
		  wren<=1'b1;
		  
    end
    SEND_7CRC:
    begin
       o_send_data_3 <=  {CRC[4],CRC[5],CRC[6],CRC[7]};
       o_sendIdl <= 1'b0;
		 address<=address+1'b1;
		 wren<=1'b1;
    end
    SEND_8CRC:
    begin
        o_send_data_3 <= {CRC[0],CRC[1],CRC[2],CRC[3]};
        o_sendIdl <= 1'b0;
		  address<=address+1'b1;
		  wren<=1'b1;
    end
    SEND_OVER:
    begin
//        o_Tx_dv_1 <= 1'b1;
        o_send_data_3 <= 4'b0000;
        o_sendDn <= 1'b1;
        o_sendIdl <= 1'b1;        
		  ifg_num <= 8'hff;
		  wren<=1'b0;
		  rden<=1'b1;
    end
	 SEND_OVER_DLY:
	 begin
//	     o_Tx_dv_1 <= 1'b0;
	     o_send_data_3<= o_send_data_3;
        ram_rdNum <= 10'h0;
        o_sendDn <= o_sendDn;
        o_sendIdl <= o_sendIdl;
		  ifg_num <= ifg_num;
		  rden<=1'b0;
	 end
    default:
    begin
		  o_send_data_3<= o_send_data_3;
//        o_Tx_dv_1 <= o_Tx_dv_1;
        o_sendDn <= o_sendDn;
        o_sendIdl <= o_sendIdl;
		  ifg_num <= ifg_num;
    end
    endcase
end
reg ram_wrEn;
always @ (posedge i_clk_125m , negedge i_rst_n)
begin
    if (!i_rst_n)
    begin
        ram_wrEn <= 1'b0;
    end
    else if (sendIrq)
    begin
        ram_wrEn <= 1'b1;
    end
	 else if(ifg_num >= 8'd30)
    begin
        ram_wrEn <= 1'b0;
    end
	 else 
		ram_wrEn <= ram_wrEn;
end

ethmac_add_crc ethmac_add_crc(
		 .i_clk(i_clk_125m), 
		 .i_rst_n(i_rst_n),
		 .i_crc_reset(sendIrq), 
		 .i_data(CRC_data), 
		 .i_crc_enable(ram_wrEn), 
		 .o_crc(CRC) 
);



endmodule
