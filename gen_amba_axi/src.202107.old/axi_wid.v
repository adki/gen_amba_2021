module axi_wid
      #(parameter WIDTH_ID= 4)
(
       input   wire                      ARESETn
     , input   wire                      ACLK
     //--------------------------------------------------------------
     , input   wire  [WIDTH_ID-1:0]      M_AWID
     , input   wire  [ 7:0]              M_AWLEN
     , input   wire                      M_AWVALID
     , input   wire                      M_AWREADY
     //--------------------------------------------------------------
     , output  wire   [WIDTH_ID-1:0]     M_WID
     , input   wire                      M_WLAST
     , input   wire                      M_WVALID
     , input   wire                      M_WREADY
);
     //--------------------------------------------------------------
     wire                  fifo_push_ready; // not used
     wire                  fifo_push_valid;
     wire  [WIDTH_ID-1:0]  fifo_push_din  = M_AWID;
     wire                  fifo_pop_ready ;
     wire                  fifo_pop_valid ; // not used
     wire  [WIDTH_ID-1:0]  fifo_pop_dout  ;
     wire                  fifo_empty     ;
     //--------------------------------------------------------------
     axi_arbiter_fifo_sync #(.FDW(WIDTH_ID), .FAW(WIDTH_ID))
     u_axi_wid_fifo_sync
     (
           .rstn     (ARESETn)
         , .clr      (1'b0   )
         , .clk      (ACLK   )
         , .wr_rdy   (fifo_push_ready)
         , .wr_vld   (fifo_push_valid)
         , .wr_din   (fifo_push_din  )
         , .rd_rdy   (fifo_pop_ready )
         , .rd_vld   (fifo_pop_valid )
         , .rd_dout  (fifo_pop_dout  )
         , .full     (               )
         , .empty    (fifo_empty     )
         , .fullN    ()
         , .emptyN   ()
         , .item_cnt ()
         , .room_cnt ()
     );
     //--------------------------------------------------------------
     assign fifo_push_din = M_AWID;
     assign fifo_push_valid = ((fifo_empty==1'b0)&&(M_AWVALID==1'b1)&&(M_AWREADY==1'b1))
                            ||((fifo_empty==1'b1)&&(M_AWVALID==1'b1)&&(M_AWREADY==1'b1)&&(M_WVALID==1'b0));
                            ||((fifo_empty==1'b1)&&(M_AWVALID==1'b1)&&(M_AWREADY==1'b1)&&(M_WVALID==1'b1)&&(M_WLAST==1'b0));
     assign M_WID = ((fifo_empty==1'b1)&&
                     (M_AWVALID==1'b1)&&(M_AWREADY==1'b1)&&
                     (M_WVALID==1'b1)) ? M_AWID : fifo_pop_dout;
     assign fifo_pop_ready = ((fifo_empty==1'b0)&&(M_WVALID==1'b1)&&(M_WREADY==1'b1));
     //--------------------------------------------------------------
endmodule
