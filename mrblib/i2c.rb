module ESP32
  class I2C
    include Constants
    def initialize(port, options={})
      @port = port
      @scl = options[:scl] || SCL0
      @scl_pullup = options[:scl_pullup] || true
      @sda = options[:sda] || SDA0
      @sda_pullup = options[:sda_pullup] || true
      @freq = options[:freq] || 100000
    end
    def init(mode=MASTER)
      raise ArgumentError.new unless [MASTER,SLAVE].include?(mode)
      raise NotImplementedError.new unless mode.eql?(MASTER)
      @mode = mode
      _init(@port, @mode, @scl, @scl_pullup, @sda, @sda_pullup, @freq)
    end
    def scan()
      raise I2CError.new unless @mode.eql?(MASTER)
      ary = []
      (0x08..0x77).each{ |addr|
        ary << addr if send("", addr) == 0
      }
      ary
    end
    def ready?(addr)
      raise I2CError.new unless @mode.eql?(MASTER)
      return send("", addr) == 0 ? true : false
    end
  end
end

class I2CError < StandardError; end
