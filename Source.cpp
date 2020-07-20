#include<iostream>
#include<fstream>
#include<cstdlib>
#include <string>
#include<vector>

using namespace std;

struct Portfel
{
	int ilosc_akcji;
	double pieniadze;
};

vector<double>dane_usd;
vector<double>dane_macd;
vector<double>dane_signal;
Portfel portfel;
bool kupione;

double Ema(int N, int dzien, bool usd)	
{										
	double suma_l = 0;					
	double suma_m = 0;
	double EMA;
	double alfa = 2.0 / (N - 1);

	for (int i = dzien, potega = 0; potega <= N; i--, potega++)
	{
		if (usd)
			suma_l += pow((1 - alfa), potega)*dane_usd[i];
		else
			suma_l += pow((1 - alfa), potega)*dane_macd[i];
		suma_m += pow((1 - alfa), potega);
	}
	EMA = suma_l / suma_m;

	return EMA;
}

void zarobki(int dzien)
{													
	int tmp;										
	double zakup;
	int dzien_macd = dzien - 26;
	int dzien_signal = dzien - 35;

	if (!kupione && dane_macd[dzien_macd]<0 && dane_macd[dzien_macd - 1]<dane_signal[dzien_signal - 1] && dane_macd[dzien_macd]>dane_signal[dzien_signal])	//kupujemy akcje
	{
		tmp = portfel.pieniadze / dane_usd[dzien];
		zakup = tmp*dane_usd[dzien];
		portfel.ilosc_akcji = tmp;
		portfel.pieniadze -= zakup;
		kupione = true;
		cout << dzien << ") Kupiono " << portfel.ilosc_akcji << " akcji za " << dane_usd[dzien] << "." << endl << endl;
	}
	else if (kupione && dane_macd[dzien_macd]>0 && dane_macd[dzien_macd - 1] > dane_signal[dzien_signal - 1] && dane_macd[dzien_macd] < dane_signal[dzien_signal])	//sprzedajemy akcje
	{
		zakup = portfel.ilosc_akcji*dane_usd[dzien];
		portfel.pieniadze += zakup;
		kupione = false;
		cout << dzien << ") Sprzedano " << portfel.ilosc_akcji << " akcji za " << dane_usd[dzien] << "." << endl;
		cout << "		Stan portfela: " << portfel.pieniadze << endl << endl;
		portfel.ilosc_akcji = 0;
	}
}

int main(int argc, char ** argv)
{
	fstream plik, mac, signal;
	portfel.pieniadze = 0;
	portfel.ilosc_akcji = 1000;
	kupione = false;
	float tmp;
	int dzien = 26;
	double tmp_plik;
	double macd, sig;

	if (argc> 1)
	{
		plik.open(argv[1]);
		mac.open("MACD.txt", ios::out);
		signal.open("SIGNAL.txt", ios::out);

		if (plik.good() && mac.good() && signal.good())
		{
			string wartosc;

			while (!plik.eof())
			{
				getline(plik, wartosc);
				tmp_plik = stod(wartosc, 0);
				dane_usd.push_back(tmp_plik);
			}
			plik.close();

			portfel.pieniadze = portfel.ilosc_akcji*dane_usd[0];
			portfel.ilosc_akcji = 0;
			tmp = portfel.pieniadze;

			for (int i = 0; i < 26; i++)
				mac << "0" << endl;
			for (int i = 0; i < 35; i++)
				signal << "0" << endl;

			for (dzien; dzien < dane_usd.size(); dzien++)
			{
				macd = Ema(12, dzien, true) - Ema(26, dzien, true);
				mac << macd << endl;
				dane_macd.push_back(macd);

				if (dane_macd.size() > 9)
				{
					sig = Ema(9, dzien - 26, false);
					signal << sig << endl;
					dane_signal.push_back(sig);
				}

				if (dane_signal.size() > 1)
				{
					zarobki(dzien);
				}
			}
			signal.close();
			mac.close(); 
		}
		else
		{
			cout << "Blad otwarcia pliku.";
			exit(0);
		}
	}
	else
	{
		cout << "Nie podano pliku wejsciowego.";
		exit(0);
	}

	cout << "Lacznie zarobiono " << portfel.pieniadze + portfel.ilosc_akcji*dane_usd[dzien - 1] - tmp << " pieniedzy." << endl;
	cout << "Wzrostu procentowo " << ((portfel.pieniadze + portfel.ilosc_akcji*dane_usd[dzien - 1]) * 100) / tmp - 100 << ".";

	return 0;
}