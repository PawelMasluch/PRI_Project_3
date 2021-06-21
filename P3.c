/* Pawel Masluch, nr albumu 262955, projekt 3.2
   Ponizszy program dziala w Linuxie, natomiast nie dziala w Windowsie  */

#define _GNU_SOURCE /* przydatne dla getline */

#include<stdio.h>
#include<stdlib.h>


const int MAX = 10; /* maksymalna liczba znakow w plikach: wejsciowym oraz wyjsciowym */

const int POZ = 10; /* podstawa systemu pozycyjnego (do wczytania liczby rownan) */


double det(long int n, long int w, int * stare_kolumny, double ** A) /* Obliczenie wyznacznika za pomoca rozwiniecia Laplaca ; zalozenie: rozwijamy wzgledem (najwyzszego) wiersza nr w */
{
	/* Znaczenie ponizszych zmiennych omowione w dalszej czesci funkcji */
	
	long int i, j, k, znak;
	
	int * nowe_kolumny;
	
	double wyn;
	
	
	
  	if( n == 1 ) /* warunek brzegowy */
	{
		return A[w][ stare_kolumny[0] ]; /* zwracamy jedyny element macierzy A */
	}
	else
  	{
	  	nowe_kolumny = (int *) malloc( ( n - 1 ) * sizeof( *nowe_kolumny ) ); /* nowa tablica z numerami kolumn */
	
	    wyn = 0; /* wynikowy wyznacznik - poczatkowo wyznacznik wynosi 0 */
	    
		znak = 1; /* ( minus jedynka ) w odpowiedniej potedze - poczatkowo mamy wartosc 1 */
	
	
	    for(i=0; i<=n-1; ++i) /* liczymy wyznacznik */
	    {
			k = 0;
	
		    for(j=0; j<=n-2; ++j) /* wypelniamy tablice nowe_kolumny */
		    {
				if(k == i) /* ignorujemy i-ta kolumne */
				{
					k++;
				}
				
		        nowe_kolumny[j] = stare_kolumny[ k++ ]; /* poza i-ta kolumna reszta numerow kolumn trafi do tablicy nowe_kolumny */
		    }
		
		
		    wyn += znak * A[w][ stare_kolumny[i] ] * det( n - 1, w + 1, nowe_kolumny, A ); /* aktualizacja wyniku */
		
		    znak = -znak; /* w kolejnym obrocie petli mamy zmiane znaku */
	
	    }
	    
	    
	    free( nowe_kolumny ); /* zwalniamy pamiec */
	    
	    
	    return wyn; /* wynikowy wyznacznik */
	}
}


void podmiana(double ** A, long int j, double * b, long int n) /* podmiana tablicy b oraz kolumny macierzy A o numerze j */
{
	double pom;
	
	long int i;
	
	
	for(i=0; i<=n-1; ++i) /* zamiana elementow */
	{
		pom = A[i][j];
		
		A[i][j] = b[i];
		
		b[i] = pom;
	}
}


int main()
{
	double Wg; /* wynacznik glowny */
	
	int var; /* czy istnieje niezerowy wyznacznik dla pewnej niewiadomej */
	
	double ** A; /* macierz */
	
	double * x; /* wynikowa tablica niewiadomych */
	
	double * b; /* tablica wyrazow wolnych */
	
	int * stare_kolumny; /* tablica do funkcji obiczajacej wyznacznik */
	
	double * W; /* tablica kolejnych wyznacznikow */
	
	long int n, i, j; /* n - liczba rownan, i,j - zmienne indeksowe */
	
	double tmp; /* zmienna pomocnicza (do wczytania wspolczynnikow ukladu rownan oraz wyrazow wolnych) */
	
	ssize_t nread; /* ile wczytano znakow w 1 linii */
	
	char * line = NULL; /* linia tekstu */
	
	char * ptr;
	
	size_t len = 0;
	
	
	
	printf( "\nPodaj nazwe pliku wejsciowego - max %d znakow\n", MAX );
	
	nread = getline( &line, &len, stdin ); /* 'line' - nazwa pliku */
	
	if( nread - 1 > MAX )
	{
		printf( "\n\nZa duzo znakow w pliku wejsciowym\n\n" );
		return 1;
	}
	
	
	
    line[nread-1] = 0;
    
	FILE *f = fopen( line, "r" );
	
	if( f == NULL ) 
	{
		printf( "\n\nBlad otwarcia pliku wejsciowego\n\n" );
		/*fclose(f);*/
		return 1;
	}
	
	
	
	nread = getline( &line, &len, f ); /* liczba rownan */

    if( nread < 0 )
    {
        printf("\n\nLinia 1: Nic nie wczytano\n\n");
        fclose(f);
        return 1;
    }



	n = strtol( line, &ptr, POZ ) ;
	
	if( ptr == line  ||  *ptr != 10 )
	{
		printf( "\n\nLinia 1: Niepoprawnie wczytana liczba rownan\n\n" );
		fclose(f);
		return 1;
	}
	/*else
	{
		printf ( "\n\nn = %ld\n\n", n );	
	}*/
	
	
	if( n <= 0 )
	{
		printf( "\n\nLinia 1: Niepoprawna liczba rownan\n\n" );
		fclose(f);
		return 1;
	}
	
	
	/*printf( "\n\nn = %ld\n\n", n );*/
	
	
	/* Tworzenie macierzy A */

  	A = (double **) malloc( n * sizeof( **A ) );

  	for(i=0; i<=n-1; ++i)
  	{
    	A[i] = ( double *) malloc( n * sizeof( *A[i] ) );
  	}
	
	
	/* Tworzenie tablicy b */
	
	b = (double *) malloc( n * sizeof( *b ) );
	
	
	
	for(i=0; i<n*n+n; ++i) /* odczyt wspolczynnikow ukladu rownan oraz wyrazow wolnych */
	{
       	/*printf( "i = %ld:\n", i  );*/

		nread = getline( &line, &len, f );
		
        if( nread < 0 )
        {
            printf( "\n\nLinia %ld: Nic nie wczytano\n\n", i+2 );
            
            
            free( b );


		  	for(i=0; i<=n-1; ++i)
		  	{
		  		free( A[i] );
			}
		
		  	free( A );
            
            fclose(f);
            
            return 1;
    	}
		
		
		tmp = strtod( line, &ptr );
		
		
		if( ptr == line  ||  *ptr != 10 )
		{
			printf( "\n\nLinia %ld: Niepoprawnie wczytany wspolczynnik\n\n", i+2 );
			
			free( b );


		  	for(i=0; i<=n-1; ++i)
		  	{
		  		free( A[i] );
			}
		
		  	free( A );
			
			fclose(f);
			
			return 1;
		}
		else
		{
			/*printf( "x = %lf\n", tmp );*/
			
			if( i%(n+1) == n ) /* wektor 'b' */
			{
				b[ i/(n+1) ] = tmp;
			}
			else /* macierz 'A' */
			{
				A[ i/(n+1) ][ i%(n+1) ] = tmp;
			}
		}
	}
	
	
	fclose(f);
	
	
	
	/* Wypisywanie kontrolne */
	
	/*printf( "\n\nMacierz A:\n" );
	
	for(i=0; i<n; ++i)
	{
		for(j=0; j<n; ++j)
		{
			printf( "%lf ", A[i][j] );
		}
		
		printf( "\n" );
	}
	
	printf( "\n\nWektor b:\n" );
	
	for(i=0; i<n; ++i)
	{
		printf( "%lf ", b[i] );
	}
		
	printf( "\n" );*/
	
	
	
	printf( "\n\nPodaj nazwe pliku wyjsciowego - max %d znakow\n", MAX );
	
	nread = getline( &line, &len, stdin ); /* nazwa pliku */
	
	/*printf( "\n\nnread = %ld\n\n", nread );*/
	
	if( nread - 1 > MAX )
	{
		printf( "\n\nZa duzo znakow w nazwie pliku wyjsciowego\n\n" );
		return 1;
	}
	
	
    line[nread-1] = 0;
    
    for(i=0; i<nread; ++i)
    {
    	if( line[i] == 32 ) /* spacja */
    	{
    		printf( "\n\nNiepoprawna nazwa pliku wyjsciowego\n\n" );
    		return 1;
		}
	}
    
    
	FILE *g = fopen( line, "w" );
	
	if( g == NULL ) 
	{
		printf( "\n\nBlad otwarcia pliku wyjsciowego\n\n" );
		
		free( b );
  	
  	
	  	for(i=0; i<=n-1; ++i)
	  	{
	  		free( A[i] );
		}
	
	  	free( A );
		
		/*fclose(g);*/
		
		return 1;
	}
	
	/* Tworzenie tablicy stare_kolumny */
  	
  	stare_kolumny = (int *) malloc( n * sizeof( *stare_kolumny ) );


	/* Wypelnienie tablicy stare_kolumny */
	
  	for(i=0; i<=n-1; ++i)
  	{
  		stare_kolumny[i] = i;
	}
	
	
	/* Tworzenie tablicy x */
	
	x = (double *) malloc( n * sizeof( *x ) );
	
	
	/* Tworzenie tablicy W */
	
	W = (double *) malloc( n * sizeof( *W ) );
	
	
	
	Wg = det( n, 0, stare_kolumny, A ); /* wyznacznik glowny */
	
	fprintf( g, "Wyznacznik glowny wynosi %lf\n\n", Wg );
	
	
	var = 0; /* jak na razie nie istnieje niezerowy wyznacznik dla pewnej niewiadomej, dlatego inicjujemy te zmienna wartoscia zero */
	
	
	fprintf( g, "Oto wyznaczniki dla kolejnych zmiennych:\n" );
	
	
	/* Obliczenie wyznacznikow dla poszczegolnych zmiennych */
	
	for(j=0; j<=n-1; ++j)
	{
		podmiana( A, j, b, n ); /* teraz j-ta kolumna macierzy A to tablica b, natomiast tablica b zawiera j-ta kolumne macierzy A  */
		
		W[j] = det( n, 0, stare_kolumny, A );
		
		fprintf( g, "Dla zmiennej x_%ld wyznacznik wynosi %lf\n", j+1, W[j] );
		
		if( W[j] != 0.0 ) /* jesli mamy niezerowy wyznacznik dla pewnej niewiadomej */
		{
			var = 1;
		}
		
		podmiana( A, j, b, n ); /* teraz j-ta kolumna macierzy A oraz tablica b sa na swoich miejscach poczatkowych */
	}
	
	
	/* Okreslenie rozwiazania */
	
	if( Wg != 0.0 ) /* wyznacznik glowny jest niezerowy, czyli uklad rownan jest oznaczony */
	{
		/* Obliczenie wynikow */
		
		for(i=0; i<=n-1; ++i)
		{
			x[i] = W[i] / Wg; /* wynik dla i-tej niewiadomej */
		}
		
		
		/* Wypisywanie wynikow */
		
		fprintf( g, "\n\nUklad rownan jest oznaczony - oto rozwiazanie:\n" );
		
		for(i=0; i<=n-1; ++i)
		{
			fprintf( g, "x_%ld = %lf\n", i + 1, x[i] );
		}
		
		fprintf( g, "\n\n" );
	}
	else /* wyznacznik wynosi zero, czyli uklad rownan jest nieoznaczony albo sprzeczny */
	{
		if( var == 1 )
		{
			fprintf( g, "\n\nUklad rownan jest sprzeczny\n" );
		}
		else /* var == 0 */
		{
			fprintf( g, "\n\nUklad rownan jest nieoznaczony albo sprzeczny\n" );
		}
	}
	
	
	fclose(g);
	
	
	/* Zwalnianie pamieci */
  	
  	free( x );
  	
  	free( b );
  	
  	free( stare_kolumny );
  	
  	free( W );
  	
  	
  	for(i=0; i<=n-1; ++i)
  	{
  		free( A[i] );
	}

  	free( A );
	
	
	/* Koniec dzialania programu */
	
	return 0;
}
