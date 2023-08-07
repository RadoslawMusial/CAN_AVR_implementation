#include <stdio.h>
#include <avr/io.h>


 #define LED (1<<PC0) // dioda jako wyjście 
 #define LED_ON PORTC &=~LED // zaświeć diode 
 #define LED_OFF PORTC |=LED // Zgaszenie 
 #define LED_TOGG PORTC ^=LED // zmiana stanu 

typedef struct 
{
   uint32_t id; 
   uint32_t mask ; 
   uint8_t ext_id:1; 
   uint8_t rtr:1; 
 }TCAN_FILTER; 
//
typedef struct {
uint32_t id; // indetyfikator wiadomości
uint8_t ext_id:1;  // ramka standardowa
uint8_t rtr:1;  // wysyłanie ramki 
uint8_t dlc2; 
uint8_t char[8]; 

}CAN_MSG; 

void can_send (uint8_t mob, CAN_MSG *msg)
{
   if (14 >mob) 
   {
      mob = 14; 
   }

   CANPAGE = (mob <<4); 
}

// inicjalizacja kontrolera 
// 1 - żądanie zaprzestania transmiji 
// 2 - oczekiwanie na zaprzestanie pracy 
// 3 - resetowanie kontrolera 
// 3 - ustawienie baud 
// 4 - włączenie kontrolera 
// 5 - oczekiwanie na bezczynności magistrali 
void can_int (void)
{
   CANGCON |= (1 << ABEQ); // żądanie zaprzestania aktualnej transmisji 


   // oczekiwanie na zaprzestanie pracy odbiornika 
   while (CANGSTA & ((1 <<RXBSY)) | (1 << TXBSY))); 

   CANGON|= (1 << SWRES); // Resetowanie układu kontrolera CAN 

   // Ustawienie czasowe dla BADUDRATE 125 kbps (dla F_CPU 16 MHZ)

   CANBT1 = 0x1E; // preskaler = 16
   CANBT2 = 0x04; 
   CANBT3 = 0x13; 


   CANGCON |= (1 << ENASTB); // włączenie kontrolera CAN 

   while (!(CANGSTA & (1 << ENFG))); // oczekiwanie na stan bezczynności magistrali (Bus IDLE) 

}



// Wysyłanie wiadomości : 

// 1 - deklaracja funkcji z odpowiednimi zmiennymi 
// 2 - zabezpieczenie przed błędnym podaniem numeru 
// 3 - MOb 
// 4 - wybór MoB do wysyłania wiadomosci 
// 5 - zerowanie rejestru MOb metodą read-modify-write 
// ZArowanie kontroli MOb i DLC 

// 6 - Zapisuwanie 11-bit ID w rejestrach wybranego Mob 



CANIDT4 = 0 ; 
CANIDT3 = 0;  
CANIDT2 = (uint8_t) (msg-id << 5) ; 
CANIDT1 = (uint8_t) (msg-id << 3) ;

if (msg->dlc > 8) msg ->dlc = 8; 


CANCDMOB |= msg->dlc; 


uint8_t *p = msg->data; 


for (uint8_t i = 0 ; i<msg->dlc;i++)
{
   CANMSG = *p++; 


   
}

CANCDMO |= (1 << CANMOB0); 

while (CANGSTA & (1 << TSBSY)); // oczekiwanie zakończenia pracy nadajnika 

void can_receive (uint8_t mob )
{
   if (mob > 14 ) 
   {
      mob = 14 ; // zabezpiecznie przed błędnym numerem MOb 
   }
   CANPAGE = (mob << 4); 

    // Odbieranie wiadomości : 
if (CANSTMOB &(1<<RXOK)) // jeśli wiadomość zostła odebrana
{
static TCAN_MSG msg ; // 
msg.dlc  = CANCDMOB  & 0x0F; // odczytywanie wartości pola DLC odebranej ramki

// // odczytywanie i zapisywanie odebranych bajtów danych do struktury 

for (uint8_t i = 0; i<msg.dlc;i++)
{
   msg.data[i] = CANMSG; 
}



}
CANSTMOB &=0; // zerowanie rejestru 
CANCDMOB = 0 ; // zerowanie rejestru DLC i MOb 
CANCMOB |= (1 << CANMOB1); //Rozpoczęcie procesu odbierania wiadomosci 
}
void set_can_filter(uint8_t mob, TCAN_FILTER *flt)
{
   if (mob > 14)
   {
mob = 14;  // zabezpieczenie przed podaniem błędnego mob ; 
   }


   CANPAGE = (mob << 4); // wybór mob do odbierania wiadomości  
   CANSTMOB &=0; // Zerowanie rejestru statusu MOb metodą read-modify-write
   CANCDMOB = 0; 
 


 // zapisanie 11-bitowej maski w rejestrach wybranego MOb : 
   CANIDM4 = 0 ; 
   CANIDM3 = 0; 
   CANIDM2 = (uint8_t) (flt->mask << 5 ); 
   CANIDM1 = (uint8_t) (flt->mask >> 3); 



    // Sekcja 3 - zapisanie 11-bit ID w rejestrach wybranego MOb
     CANIDT4 = 0 ; 
   CANIDT3 = 0; 
   CANIDT2 = (uint8_t) (flt->id << 5 ); 
   CANIDT1 = (uint8_t) (flt->id >> 3); 

CANCDMOB |= (1<< CANMOB1); // rozpoczęcie procesu odbierania wiadomości
 
     
}

int main ()
{
   DDRC | = LED; // ustaweienie pinu jako wyjście 
   LED_OFF; 
   can_init(); // inicjalizacja CAN ; 

   TCAN_FILTER filter ; 

}
