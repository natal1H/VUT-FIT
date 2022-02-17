/**
 * @file bit_array.h
 * @author Natalia Holkova (xholko02), FIT
 * @date 20.3.2018
 * @brief Definuje datovu strukturu pole bitov a makra pre pracu s nim
 *
 * Riesenie IJC-DU1, prklad a)
 * Prelozene: gcc 5.4
 */

#include <limits.h>
#include "error.h"

#ifndef _BIT_ARRAY_H_
	#define _BIT_ARRAY_H_

	/**
	 * @brief Pole bitov
	 */
	typedef unsigned long bit_array_t[];

	/**
	 * @brief Definuje a nuluje pole bitov
	 *
	 * @param meno_pola pole bitov
	 * @param velkost velkost pola v BITOCH
	 */
	#define bit_array_create(meno_pola, velkost) unsigned long meno_pola[velkost/ (sizeof(unsigned long)* CHAR_BIT) + \
		1 + ( (velkost % (sizeof(unsigned long) * CHAR_BIT) ) ? 1 : 0)] \
		= {velkost,0} // nastavenie zvysnych prvkov na 0

	/**
	 * @brief Vrati deklarovanu velkost pola v bitoch
	 *
	 * @param meno_pola pole bitov
	 * @return velkost pola v bitoch
	 */
	#ifdef USE_INLINE
		static inline unsigned long bit_array_size(bit_array_t meno_pola) {
			return meno_pola[0];
		}
	#else
		#define bit_array_size(meno_pola) meno_pola[0]
	#endif

	/**
	 * @brief Nastavi zadany bit na hodnotu zadaneho vyrazu
	 *
	 * @param meno_pola pole bitov
	 * @param index index bitu na nastavenie
	 * @param vyraz vyraz, na ktory nastavit bit
	 */
	#ifdef USE_INLINE
		static inline void bit_array_setbit(bit_array_t meno_pola, unsigned long index, int vyraz) {
			/* Kontrola hranic pola */
			if (index >= bit_array_size(meno_pola) )
				error_exit("Index %lu je mimo platny rozsah <0,%lu>!\n", index, bit_array_size(meno_pola)-1);
			/* Nastavenie bitu na 1 */
			if (vyraz)
				meno_pola[1 + index / (sizeof(unsigned long) * CHAR_BIT)] |=
				1UL << (index % (sizeof(unsigned long) * CHAR_BIT) ) ;
			/* Nastavenie bitu na 0 */
			else
				meno_pola[1 + index/ (sizeof(unsigned long) * CHAR_BIT) ] &=
				~(1UL << (index % (sizeof(unsigned long) * CHAR_BIT) ) );
		}
	#else
		#define bit_array_setbit(meno_pola, index, vyraz) do { \
				if(index >= bit_array_size(meno_pola) ) { \
					error_exit("A Index %lu je mimo platny rozsah <0,%lu>!\n", index, bit_array_size(meno_pola)-1); \
				} \
				else { \
					 vyraz ? \
					( meno_pola[1 + index/ (sizeof(unsigned long) * CHAR_BIT) ] |= 1UL << (index % (sizeof(unsigned long)*CHAR_BIT)) ) : \
					( meno_pola[1 + index/ (sizeof(unsigned long) * CHAR_BIT) ] &= ~(1UL << (index % (sizeof(unsigned long)*CHAR_BIT))) ); \
				} \
			} \
			while(0)
	#endif

	/**
	 * @brief Ziska hodnotu zadaneho bitu
	 *
	 * @param meno_pola pole bitov
	 * @param index index bit na ziskanie
	 * @return hodnota bitu (0 alebo 1)
	 */
	#ifdef USE_INLINE
		static inline int bit_array_getbit(bit_array_t meno_pola, unsigned long index) {
			/* Kontrola hranic pola */
			if (index >= bit_array_size(meno_pola) ) {
				error_exit("Index %ld mimo rozsah 0..%ld\n", index, bit_array_size(meno_pola)-1); return -1;
			}
			/* Vratenie bitu */
			else
				return ( (meno_pola[1 + index/sizeof(unsigned long) / CHAR_BIT] &
						(1LU << (index%(sizeof(unsigned long) * CHAR_BIT)) ) ) != 0);
			}
	#else
		#define bit_array_getbit(meno_pola, index) \
			(index >= bit_array_size(meno_pola)) ? \
				error_exit("Index %ld mimo rozsah 0..%ld\n", (unsigned long)index, (unsigned long)bit_array_size(meno_pola)-1), -1 \
			: \
			( (meno_pola[1 + index/sizeof(unsigned long) / CHAR_BIT] & \
						(1LU << (index%(sizeof(unsigned long) * CHAR_BIT)) ) ) != 0)
	#endif

#endif
