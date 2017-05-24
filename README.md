mruby-esp32-i2c
============

I2C library for mruby-esp32.

## Installation
Add the line below to your `build_config.rb`:

```ruby
  conf.gem :github => 'mruby-esp32/mruby-esp32-i2c'
```

## Example

Example of controlling the LCD display with ST7032 controller.

```ruby
include ESP32

class ST7032
  def initialize(i2c, addr=0x3e, cols=8)
    @i2c = i2c
    @addr = addr
    @cols = cols
  end
  def init
    # see controller data sheet
    @i2c.send("\x00\x38\x39\x14\x70\x56\x6c", @addr)
    System.delay(200)
    @i2c.send("\x00\x38\x0c\x01", @addr)
    System.delay(1)
  end
  def ready?
    @i2c.ready?(@addr)
  end
  def contrast(n)
    c1 = (0x70 | (n & 0x0f)).chr
    c2 = (0x54 | (n >> 4) & 0x03).chr
    @i2c.send(["\x00\x39", c1, c2, "\x38"].join, @addr)
  end
  def clear
    @i2c.send("\x00\x01", @addr)
  end
  def print(str)
    clear
    @i2c.send("\x00\x80", @addr)
    @i2c.send(["\x40", str[0,@cols]].join, @addr)
    if str.length > @cols
      @i2c.send("\x00\xc0", @addr);
      @i2c.send(["\x40", str[@cols,@cols]].join, @addr)
    end
  end
end

i2c = I2C.new(I2C::PORT0).init(I2C::MASTER)

p i2c.scan

lcd = ST7032.new(i2c)

until lcd.ready?
  System.delay(1000)
end

lcd.init

lcd.print("Hello, world!")

System.delay(3000)

(0..63).each {|c|
  lcd.contrast(c)
  lcd.print("c=#{c}")
  System.delay(100)
}

lcd.contrast(32)
lcd.clear

i2c.deinit
```



## License

Copyright (c) 2017 YAMAMOTO Masaya

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in 
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.
