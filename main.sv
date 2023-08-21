module main(
  input logic clk,
  input logic reset,
  input logic[2:0] switches,

  output logic[6:0] display_5,
  output logic[6:0] display_4,
  output logic[6:0] display_3,
  output logic[6:0] display_2,
  output logic [6:0] display_1,
  output logic [6:0] display_0
);

  logic clk_aux;
  logic reset_aux;
  logic [2:0] switches_aux;

  logic [3:0] display_5_aux;
  logic [3:0] display_4_aux;
  logic [3:0] display_3_aux;
  logic [3:0] display_2_aux;
  logic [3:0] display_1_aux;
  logic [3:0] display_0_aux;

  platform plat(
    .clk_clk(clk_aux),
    .reset_reset_n(reset_aux),
    .switches_0_external_connection_export(switches_aux),
    .display_5_external_connection_export(display_5_aux),
    .display_4_external_connection_export(display_4_aux),
    .display_3_external_connection_export(display_3_aux),
    .display_2_external_connection_export(display_2_aux),
    .display_1_external_connection_export(display_1_aux),
    .display_0_external_connection_export(display_0_aux)
  );

  display_driver display_5_driver(
    .data(display_5_aux),
    .segment(display_5)
  );

  display_driver display_4_driver(
    .data(display_4_aux),
    .segment(display_4)
  );

  display_driver display_3_driver(
    .data(display_3_aux),
    .segment(display_3)
  );

  display_driver display_2_driver(
    .data(display_2_aux),
    .segment(display_2)
  );

  display_driver display_1_driver(
    .data(display_1_aux),
    .segment(display_1)
  );

  display_driver display_0_driver(
    .data(display_0_aux),
    .segment(display_0)
  );

  always_comb begin
    clk_aux = clk;
    reset_aux = reset;
    switches_aux = switches;
  end
endmodule