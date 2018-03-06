module rec_test(
   input  wire         rec_data,
	input  wire         i_clk_125m,
	input  wire         recIrq,
   input  wire         i_rst_n
);
parameter IDLE                  = 5'b00000;
parameter REC_START             = 5'b00001;
parameter REC_data              = 5'b00011;
parameter REC_c_data            = 5'b00010;
parameter WAIT_CRC              = 5'b00110;
parameter REC_1CRC              = 5'b00111;
parameter REC_2CRC              = 5'b00101;
parameter REC_3CRC              = 5'b00100;
parameter REC_4CRC              = 5'b01100;
parameter REC_5CRC              = 5'b01110;
parameter REC_6CRC              = 5'b01111;
parameter REC_7CRC              = 5'b01101;
parameter REC_8CRC              = 5'b01001;
parameter COMPARE               = 5'b01000;
parameter SAVE_DATA             = 5'b11000;
parameter REC_OVER              = 5'b11001;
//parameter SEND_8CRC             = 5'b11011;
//parameter SEND_OVER             = 5'b11010;
//parameter SEND_OVER_DLY         = 5'b11110;
reg [15:0] CRC_data;
reg [4:0] fsm_cs;
reg [4:0] fsm_ns;
reg [3:0] ram_data;
reg wren;
reg [7:0] address;
reg [7:0] NUM;
reg [3:0] e_data[0:15];
reg [3:0] data [16:45];
reg [3:0] c_data[46:53];
reg [7:0] crc_num;
reg [3:0] crc[0:7];
wire [31:0] CRC; 
reg [7:0] C_NUM;
reg [7:0] NUM1;
reg [7:0] SAVE_NUM;
reg wrong;
reg ram_wrEn;
always @(posedge i_clk_125m,negedge i_rst_n)
begin
	if(!i_rst_n)
		CRC_data <= 16'hffff;
	else if(fsm_ns == WAIT_CRC && crc_num < 8'd30)
		CRC_data <= data[crc_num];
	else 
      CRC_data <= 16'hffff;
end
//always @ (posedge i_clk_125m , negedge i_rst_n)
//begin
//    if (!i_rst_n)
//    begin
//        ram_wrEn <= 1'b0;
//    end
//    else if (recIrq)
//    begin
//        ram_wrEn <= 1'b1;
//    end
//	 else if(crc_num >= 8'd46)
//    begin
//        ram_wrEn <= 1'b0;
//    end
//	 else 
//		ram_wrEn <= ram_wrEn;
//end
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
        if (recIrq)
		  fsm_ns = REC_START;
		  else 
		  fsm_ns = IDLE;
	 end      
    REC_START:
    begin
	 if(NUM>8'd15)    
		  fsm_ns = REC_data;
	 else
	     fsm_ns = REC_START;
	 end
	 REC_data:
    begin    
		  if(NUM>8'd45)    
		  fsm_ns = REC_c_data;
		  else
	     fsm_ns = REC_START;
	 end	  
	 REC_c_data:
    if(NUM>8'd53)    
		  fsm_ns = WAIT_CRC;	  
    WAIT_CRC:
	 if(crc_num>8'd46)
	     fsm_ns = REC_1CRC;
	 else
	     fsm_ns = WAIT_CRC;
    REC_1CRC:
        fsm_ns = REC_2CRC;
    REC_2CRC:
        fsm_ns = REC_3CRC;
    REC_3CRC:
        fsm_ns = REC_4CRC;
    REC_4CRC:
        fsm_ns = REC_5CRC;
    REC_5CRC:
        fsm_ns = REC_6CRC;
    REC_6CRC:
        fsm_ns = REC_7CRC;
    REC_7CRC:
        fsm_ns = REC_8CRC;
    REC_8CRC:
        fsm_ns = COMPARE;
	 COMPARE:
	 if(C_NUM>8'h07&&!wrong)    
		  fsm_ns = SAVE_DATA;
	 else if(C_NUM<8'h08&&!wrong) 
	     fsm_ns = COMPARE;
	 else if(wrong)
        fsm_ns = IDLE;	 
	 SAVE_DATA:
    if(SAVE_NUM>8'd30)
        fsm_ns = REC_OVER;
	 else
        fsm_ns =SAVE_DATA; 
    REC_OVER:
        fsm_ns = IDLE;
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
    NUM<=8'h0;
	 crc_num<=8'h0;
	 NUM1<=8'd46;
	 wrong<=1'b0;
    end
    else 
    case (fsm_ns)
    IDLE:
    begin
    NUM<=8'h0;
	 crc_num<=8'h0;
	 wrong<=1'b0;
	 NUM1<=8'd46;
    end 
    REC_START:
    begin
    e_data[NUM]<=rec_data;
	 NUM<=NUM+1'b1;
    end
    REC_data:
    begin
    data[NUM]<=rec_data;
	 NUM<=NUM+1'b1;
    end
    REC_c_data:
    begin
    c_data[NUM]<=rec_data;
	 NUM<=NUM+1'b1;
    end	 
    WAIT_CRC:
    begin
    crc_num<=crc_num+1'b1;
    end                        
    REC_1CRC:
    begin
        crc[0] <= {CRC[28],CRC[29],CRC[30],CRC[31]};
    end
    REC_2CRC:
    begin
        crc[1] <= {CRC[24],CRC[25],CRC[26],CRC[27]};	  
    end
    REC_3CRC:
    begin
        crc[2] <= {CRC[20],CRC[21],CRC[22],CRC[23]};
    end
    REC_4CRC:
    begin
        crc[3] <= {CRC[16],CRC[17],CRC[18],CRC[19]};	  
    end
    REC_5CRC:
    begin
        crc[4] <= {CRC[12],CRC[13],CRC[14],CRC[15]};
    end
    REC_6CRC:
    begin
        crc[5] <= {CRC[8],CRC[9],CRC[10],CRC[11]};		  
    end
    REC_7CRC:
    begin
        crc[6] <=  {CRC[4],CRC[5],CRC[6],CRC[7]};
    end
    REC_8CRC:
    begin
        crc[7] <= {CRC[0],CRC[1],CRC[2],CRC[3]};
    end
	 COMPARE:
	 begin
	     if(crc[C_NUM]==c_data[NUM1])
		  begin
		  C_NUM<=C_NUM+1'b1;
		  NUM1<=NUM1+1'b1;
		  end
		  else
		  begin
		  wrong<=1'b1;
		  end
	 end
	 SAVE_DATA:
	 begin
	     ram_data<=data[SAVE_NUM];
		  SAVE_NUM<=SAVE_NUM+1'b1;
		  wren<=1'b1;
		  address<=address+1'b1;
	 end
    REC_OVER:
    begin
    NUM<=8'h0;
	 crc_num<=8'h0;
	 wrong<=1'b0;
    end

	 
    default:
    begin
    NUM<=8'h0;
	 crc_num<=8'h0;
	 wrong<=1'b0;
    end
    endcase
end
always @ (posedge i_clk_125m , negedge i_rst_n)
begin
    if (!i_rst_n)
    begin
        ram_wrEn <= 1'b0;
    end
    else if (recIrq)
    begin
        ram_wrEn <= 1'b1;
    end
	 else if(crc_num >= 8'd30)
    begin
        ram_wrEn <= 1'b0;
    end
	 else 
		ram_wrEn <= ram_wrEn;
end

ethmac_add_crc ethmac_add_crc(
		 .i_clk(i_clk_125m), 
		 .i_rst_n(i_rst_n),
		 .i_crc_reset(recIrq), 
		 .i_data(CRC_data), 
		 .i_crc_enable(ram_wrEn), 
		 .o_crc(CRC) 
);
wire q;

RAM RAM(
        .clock(i_clk_125m),
		  .data(ram_data),
		  .address(address),
		  .wren(wren),
		  .q(q)
);




endmodule 