module timers(
  input logic clk,
  input logic reset,
  input logic[2:0] switches,

  output logic[6:0] minutes_display_0,
  output logic[6:0] minutes_display_1,
  output logic[6:0] seconds_display_0,
  output logic[6:0] seconds_display_1
);

  platform plat(
    .clk_clk(clk),
    .reset_reset_n(reset),
	  .switches_0_external_connection_export(switches),
    .minutes_display_0_external_connection_export(minutes_display_0),
    .minutes_display_1_external_connection_export(minutes_display_1),
    .seconds_display_0_external_connection_export(seconds_display_0),
    .seconds_display_1_external_connection_export(seconds_display_1)
  );

endmodule;