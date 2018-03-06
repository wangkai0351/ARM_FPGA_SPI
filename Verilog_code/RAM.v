module RAM(
    input     [7:0]  address,
    input            clock,
    input     [3:0]  data,
    input            wren,
	 input            rden,
    output   reg [3:0]  q
);
reg [3:0]mem[0:127];
wire q1;
RAM1P    RAM1P_inst  (
    .address ( address ),
    .clock ( clock ),
    .data ( data ),
    .wren ( wren ),
    .q ( q1)
    );

always@(posedge clock)
begin
  if(wren)
     begin
	  mem[address]<=data;
	  end
	  else
	  mem[address]<=1'b0;

end
reg [7:0] NUM;
always@(posedge clock) 
begin
  if(rden&&NUM<8'h91)
  begin
  q<=mem[NUM];
  NUM<=NUM+8'h01;
  end
  else
  begin
  q<=q;
  NUM<=8'h00;
  
  end
  
end	 
endmodule 