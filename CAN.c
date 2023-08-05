#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>



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
void can_send (void)
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