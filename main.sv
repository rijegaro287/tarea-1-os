module main(
  input logic clk,
  input logic reset,
  input logic[2:0] switches,

  output logic[6:0] minutes_display_0,
  output logic[6:0] minutes_display_1,
  output logic[6:0] seconds_display_0,
  output logic[6:0] seconds_display_1,
  output logic [6:0] millis_display_0,
  output logic [6:0] millis_display_1
);

  logic clk_aux;
  logic reset_aux;
  logic [2:0] switches_aux;

  logic [3:0] minutes_display_0_aux;
  logic [3:0] minutes_display_1_aux;
  logic [3:0] seconds_display_0_aux;
  logic [3:0] seconds_display_1_aux;
  logic [3:0] millis_display_0_aux;
  logic [3:0] millis_display_1_aux;

  platform plat(
    .clk_clk(clk_aux),
    .reset_reset_n(reset_aux),
    .switches_0_external_connection_export(switches_aux),
    .minutes_display_1_external_connection_export(minutes_display_1_aux),
    .minutes_display_0_external_connection_export(minutes_display_0_aux),
    .seconds_display_1_external_connection_export(seconds_display_1_aux),
    .seconds_display_0_external_connection_export(seconds_display_0_aux),
    .millis_display_1_external_connection_export(millis_display_1_aux),
    .millis_display_0_external_connection_export(millis_display_0_aux)
  );

  display_driver minutes_display_1_driver(
    .data(minutes_display_1_aux),
    .segment(minutes_display_1)
  );
  display_driver minutes_display_0_driver(
    .data(minutes_display_0_aux),
    .segment(minutes_display_0)
  );

  display_driver seconds_display_1_driver(
    .data(seconds_display_1_aux),
    .segment(seconds_display_1)
  );
  display_driver seconds_display_0_driver(
    .data(seconds_display_0_aux),
    .segment(seconds_display_0)
  );

  display_driver millis_display_1_driver(
    .data(millis_display_1_aux),
    .segment(millis_display_1)
  );
  display_driver millis_display_0_driver(
    .data(millis_display_0_aux),
    .segment(millis_display_0)
  );

  always_comb begin
    clk_aux = clk;
    reset_aux = reset;
    switches_aux = switches;
  end
endmodule