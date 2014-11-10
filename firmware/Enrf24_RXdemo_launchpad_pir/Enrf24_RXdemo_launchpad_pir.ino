#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>
#include <SPI.h>

// stellaris launchpad (spi1 <-> nrf24l01)
Enrf24 radio(PE_1, PE_2, PE_3);  // PE1 = CE, PE2 = CSN, PE3 = IRQ
const uint8_t rxaddr[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0x01 };

const char *str_on = "ON";
const char *str_off = "OFF";

void dump_radio_status_to_serialport(uint8_t);

char* cmd_name[]={
  "CONFIG",
  "EN_AA",
  "EN_RXADDR",
  "SETUP_AW",
  "SETUP_RETR",
  "RF_CH",
  "RF_SETUP",
  "STATUS",
  "OBSERVE_TX",
  "CD",
  "RX_ADDR_P0",
  "RX_ADDR_P1",
  "RX_ADDR_P2",
  "RX_ADDR_P3",
  "RX_ADDR_P4",
  "RX_ADDR_P5",
  "TX_ADDR",
  "RX_PW_P0",
  "RX_PW_P1",
  "RX_PW_P2",
  "RX_PW_P3",
  "RX_PW_P4",
  "RX_PW_P5",
  "FIFO_STATUS",
  "N/A",
  "N/A",
  "N/A",
  "N/A",
  "DYNPD",
  "FEATURE"
};

void setup() {
  Serial.begin(9600);

  SPI.setModule(1);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(1); // MSB-first

  radio.begin();  // Defaults 1Mbps, channel 0, max TX power
  dump_radio_status_to_serialport(radio.radioState());

  radio.setRXaddress((void*)rxaddr);
  /*  
   pinMode(P1_0, OUTPUT);
   digitalWrite(P1_0, LOW);
   */
  radio.enableRX();  // Start listening

  register_status();
}

void loop() {
  char inbuf[33];

  dump_radio_status_to_serialport(radio.radioState());  // Should show Receive Mode

    while (!radio.available(true))
    ;
  if (radio.read(inbuf)) {
    Serial.print("Received packet: ");
    Serial.println(inbuf);
    /*
    if (!strcmp(inbuf, str_on))
     digitalWrite(P1_0, HIGH);
     if (!strcmp(inbuf, str_off))
     digitalWrite(P1_0, LOW);
     */
  }
}

void dump_radio_status_to_serialport(uint8_t status)
{
  Serial.print("Enrf24 radio transceiver status: ");
  switch (status) {
  case ENRF24_STATE_NOTPRESENT:
    Serial.println("NO TRANSCEIVER PRESENT");
    break;

  case ENRF24_STATE_DEEPSLEEP:
    Serial.println("DEEP SLEEP <1uA power consumption");
    break;

  case ENRF24_STATE_IDLE:
    Serial.println("IDLE module powered up w/ oscillators running");
    break;

  case ENRF24_STATE_PTX:
    Serial.println("Actively Transmitting");
    break;

  case ENRF24_STATE_PRX:
    Serial.println("Receive Mode");
    break;

  default:
    Serial.println("UNKNOWN STATUS CODE");
  }
}

void register_status(void)
{
  uint8_t reg;


  for (reg = 0; reg <=0x1d ; reg++)
  {

    if (reg == 0xa || reg == 0xb || reg == 0x10)
    {
      uint8_t value[5];
      radio._readRegMultiLSB(reg, &value[0],5);
      //value = radio.readReg(reg);
      //Serial.print("register:0x");
      //if (reg<0x10)
      //  Serial.print("0");
      //Serial.print(reg, HEX);
      //Serial.print(" value:");
      Serial.print("\"");
      Serial.print(cmd_name[reg]);
      Serial.print("\" ");
      uint8_t i;
      for (i=0;i<5;i++)
      {
        Serial.print("0x");
        if (value[i]<0x10)
          Serial.print("0");
        Serial.print(value[i],HEX);
        //add space between values if not last value. last value needs a newline
        if (i < 4)
        {
          Serial.print(" ");
        }
        else
        {
          Serial.println("");
        }
      }
    }
    else
    {
      uint8_t value;
      value = radio.readReg(reg);
      //Serial.print("register:0x");
      //if (reg<0x10)
      //  Serial.print("0");
      //Serial.print(reg, HEX);
      //Serial.print(" value:0x");
      Serial.print("\"");
      Serial.print(cmd_name[reg]);
      Serial.print("\"");
      Serial.print(" 0x");
      if (value<0x10)
        Serial.print("0");
      Serial.println(value,HEX);
    }
  }
}


