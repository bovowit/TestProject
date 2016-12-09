
#include <SPI.h>

#define SS 22
int idx = 0;
char SendByte[100] = {'t', 'e', 's', 't', '-','s','l','a','v','e','r','!', };
char ReadByte[100] = {'0', };

void slaveBegin(uint8_t _pin) {
 SPI.begin(_pin);
 REG_SPI0_CR = SPI_CR_SWRST;     // reset SPI
 REG_SPI0_CR = SPI_CR_SPIEN;     // enable SPI
 REG_SPI0_MR = SPI_MR_MODFDIS;     // slave and no modefault
 //REG_SPI0_CSR = 0x80;    // DLYBCT=0, DLYBS=0, SCBR=0, 8 bit transfer
 REG_SPI0_CSR = 0x00;    // DLYBCT=0, DLYBS=0, SCBR=0, 8 bit transfer

#if USE_SAM3X_DMAC
  pmc_enable_periph_clk(ID_DMAC);
  dmac_disable();
  DMAC->DMAC_GCFG = DMAC_GCFG_ARB_CFG_FIXED;
  dmac_enable();
#if USE_SAM3X_BUS_MATRIX_FIX
  MATRIX->MATRIX_WPMR = 0x4d415400;
  MATRIX->MATRIX_MCFG[1] = 1;
  MATRIX->MATRIX_MCFG[2] = 1;
  MATRIX->MATRIX_SCFG[0] = 0x01000010;
  MATRIX->MATRIX_SCFG[1] = 0x01000010;
  MATRIX->MATRIX_SCFG[7] = 0x01000010;
#endif  // USE_SAM3X_BUS_MATRIX_FIX
#endif  // USE_SAM3X_DMAC

}
void setup() 
{
   SPI.begin(SS);
  
   pinMode(SS, INPUT); // ????
   Serial.begin(115200);
   SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
   Serial.println("Slaver Started..");

   //slaveBegin(SS);
}

void loop()
{
  //char s = SPI.transfer(SS, 0x00);
  //Serial.print("received start char "); Serial.println(s);
  //if(s == 'K')
  if(digitalRead(SS) == LOW)
  { 
      //char sr = SPI.transfer(SS, 'S');
      //if(sr == 'a')
      {
        Serial.println("request data send to master : Ox5B... ");
        for(int i = 0; i < 10; i++)
        {
          char ret = SPI.transfer(SS, SendByte[i], SPI_CONTINUE);
          Serial.print(idx); Serial.print(" : "); Serial.print(SendByte[i]); Serial.print(" : "); Serial.println(ret);
          delay(10);
        }
         SPI.transfer(SS, 0x00);    // continue mode 종료
         delay(1000);
      }
       idx++;
      //memset(ReadByte, 0, 100);    
  }

}
