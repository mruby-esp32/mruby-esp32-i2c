#include <mruby.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/value.h>
#include <mruby/variable.h>

#include "driver/i2c.h"

#define E_I2C_ERROR (mrb_class_get(mrb, "I2CError"))

static mrb_value
mrb_esp32_i2c_init(mrb_state *mrb, mrb_value self)
{
  mrb_int mode, port, scl, sda, freq;
  mrb_bool scl_pullup, sda_pullup;
  i2c_config_t conf;

  mrb_get_args(mrb, "iiibibi", &port, &mode, &scl, &scl_pullup, &sda, &sda_pullup, &freq);

  conf.mode = mode;
  conf.scl_io_num = scl;
  conf.sda_io_num = sda;
  conf.scl_pullup_en = scl_pullup;
  conf.sda_pullup_en = sda_pullup;
  conf.master.clk_speed = freq;
  conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

  i2c_param_config(port, &conf);
  i2c_driver_install(port, mode, 0, 0, 0);

  return self;
}

static mrb_value
mrb_esp32_i2c_deinit(mrb_state *mrb, mrb_value self)
{
  mrb_value port;

  port = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@port"));
  i2c_driver_delete(mrb_fixnum(port));
  return mrb_nil_value();
}

static mrb_value
mrb_esp32_i2c_send(mrb_state *mrb, mrb_value self) {
    mrb_value data, port;
    mrb_int addr;
    i2c_cmd_handle_t cmd;
    esp_err_t err;

    mrb_get_args(mrb, "Si", &data, &addr);
    port = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@port"));

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1 ) | I2C_MASTER_WRITE, 1);
    if (!mrb_nil_p(data)) {
      i2c_master_write(cmd, RSTRING_PTR(data), RSTRING_LEN(data), 1);
    }
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(mrb_fixnum(port), cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return mrb_fixnum_value(err);
}

void
mrb_mruby_esp32_i2c_gem_init(mrb_state* mrb)
{
  struct RClass *esp32, *i2c, *constants;

  esp32 = mrb_define_module(mrb, "ESP32");

  i2c = mrb_define_class_under(mrb, esp32, "I2C", mrb->object_class);
  mrb_define_method(mrb, i2c, "_init", mrb_esp32_i2c_init, MRB_ARGS_REQ(7));
  mrb_define_method(mrb, i2c, "deinit", mrb_esp32_i2c_deinit, MRB_ARGS_NONE());
  mrb_define_method(mrb, i2c, "send", mrb_esp32_i2c_send, MRB_ARGS_REQ(2));

  constants = mrb_define_module_under(mrb, i2c, "Constants");

  mrb_define_const(mrb, constants, "PORT0", mrb_fixnum_value(I2C_NUM_0));
  mrb_define_const(mrb, constants, "PORT1", mrb_fixnum_value(I2C_NUM_1));

  mrb_define_const(mrb, constants, "SDA0", mrb_fixnum_value(GPIO_NUM_18));
  mrb_define_const(mrb, constants, "SCL0", mrb_fixnum_value(GPIO_NUM_19));
  mrb_define_const(mrb, constants, "SDA1", mrb_fixnum_value(GPIO_NUM_25));
  mrb_define_const(mrb, constants, "SCL1", mrb_fixnum_value(GPIO_NUM_26));

  mrb_define_const(mrb, constants, "MASTER", mrb_fixnum_value(I2C_MODE_MASTER));
  mrb_define_const(mrb, constants, "SLAVE", mrb_fixnum_value(I2C_MODE_SLAVE));
}

void
mrb_mruby_esp32_i2c_gem_final(mrb_state* mrb)
{
}
