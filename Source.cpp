# include "Header.h"
#include <math.h>

Punct::Punct(int x, int y, char c){
	this->x = x;
	this->y = y;
	this->c = c;
}
Punct::Punct(const Punct& P){
	x = P.x;
	y = P.y;
	c = P.c;
}
double Punct::distanta(const Punct & P1, const Punct& P2) {
	return sqrt((P1.x - P2.x)*(P1.x - P2.x) + (P1.y - P2.y)*(P1.y - P2.y));
}
int Punct::operator ==(const Punct& P){
	if ((x == P.x) && (y == P.y))
		return 1;
	return 0;
}
int Punct::operator !=(const Punct& P){
	if (*this == P)return 0;
	return 1;
}
ostream& operator <<(ostream & out, const Punct& P){
	out << P.c <<"(" << P.x << "," << P.y<<")";
	return out;
}
istream& operator >>(istream & in, Punct & P)
{
	in >> P.c>>P.x>>P.y;
	return in;
}
Figura::Figura(const int n,const Punct* P)
{
	this->n = n;
	this->P = new Punct[n];
	for (int i = 0; i<n; i++)
		this->P[i] = P[i];
}
Figura::Figura(const Figura& f)
{
	n = f.n;
	P = new Punct[f.n];
	for (int i = 0; i < f.n; i++)
		P[i] = f.P[i];
}
Figura::~Figura() {
	if (P)delete[]P;
}
Figura Figura::operator+(const Punct& P) {
	Punct* pp=new Punct[n+1];
	for (int i = 0; i<n; i++)
		pp[i] = this->P[i];
	pp[n] = P;
	Figura rez(n + 1, pp);
	delete pp;
	return rez;
}
Figura Figura::operator-(const Punct& P) {
	Punct* pp = new Punct[n];
	int nn = 0;
	for (int i = 0; i < n; i++) {
		if(this->P[i]!=P)pp[nn++] = this->P[i];
	}
	Figura rez(nn, pp);
	delete pp;
	return rez;
}
Figura& Figura::operator=(const Figura& f) {
	if (this != &f) {
		n = f.n;
		if (P)delete[] P;
		P = new Punct[f.n];
		for (int i = 0; i < n; i++) {
			P[i] = f.P[i];
		}
	}
	return *this;
}
double Figura::Perimetru(){
	double S = 0;
	for (int i = 1; i<n; i++)
		S += Punct::distanta(P[i-1],P[i]);
	if(n>1)S+= Punct::distanta(P[0], P[n-1]);
	return S;
}
double Figura::Aria(){
	return 0;//de implementat
}
int Figura::InInterior(const Punct& P){
	return 0;//de implementat
}
ostream& operator <<(ostream & out, const Figura& f) {
	out << "[";
	for (int i = 0; i < f.n;i++) out << (i==0?"":",")<<f.P[i].getText();
	out << "]";
	return out;
}
Dreptunghi::Dreptunghi(const Punct& SS, const Punct& DJ) :Figura(2,new Punct[2]{SS,DJ}){}
double Dreptunghi::Aria() {
	return (P[1].getX() - P[0].getX()) * (P[1].getY() - P[0].getY());
}
ostream& operator <<(ostream & out, const Dreptunghi& f) {
	out << "D:"<< f.P[0].getText()<<","<<f.P[1].getText();
	return out;
}
ostream& operator <<(ostream & out, const Triunghi& f) {
	out << "T:" << f.P[0].getText() << "," << f.P[1].getText() << "," << f.P[2].getText();
	return out;
}
Cerc::Cerc(const Punct& O, const double& r) : Figura(1, new Punct[1]{ O }) {
	this->r = r;
}
double Cerc::Perimetru() {
	return 2 * PI*r;
}
double Cerc::Aria() {
	return PI*r*r;
}
int Cerc::InInterior(const Punct& P) {
	return Punct::distanta(*this->P,P)<=r;
}
Cerc::operator Dreptunghi(){
	return Dreptunghi(Punct(P[0].getX()-(int)r,P[0].getY()-(int)r,'A'), Punct(P[0].getX() + (int)r, P[0].getY() + (int)r,'B'));
}
Triunghi::Triunghi(const Punct& A, const Punct& B, const Punct& C) :Figura(3, new Punct[3]{ A,B,C }) {}

void scrie(Figura* f) {
    Dreptunghi* d = dynamic_cast<Dreptunghi*>(f);
    if (d) {
        std::cout << *d<<"\n";
        return;
    }
    Triunghi* t = dynamic_cast<Triunghi*>(f);
    if (t) {
        std::cout << *t<<"\n";
        return;
    }
    Cerc* c = dynamic_cast<Cerc*>(f);
    if (c) {
        std::cout << *c<<"\n";
        return;
    }
    std::cout << *f<< "\n";
}

int main() {
	Punct A(0,0,'A'), B(100, 0,'B'), C(100, 100,'C');
	Figura f(3, new Punct[3]{ A, B, C });
	Cerc c(B, 50);
	Dreptunghi d = (Dreptunghi)c;
	Triunghi t(A, B, C);
	cout << d.Aria() << endl;//de urmarit cu si fara virtual
	f = t + C;
	cout << f << endl;
	f = t - C;
	cout << t<< endl;
	//pentru conversii
	cout << "===========================" << endl;
	f = t;
	//t = f;//EROARE !!!!!!!
	Figura* pF = new Figura (1, new Punct);
	Dreptunghi* pD = &d;
	delete pF;
	pF = &c;
	pF = pD;
	//pD = pF;//EROARE !!!!!!!
	pF = &t;
	cout << *pF << endl;
	cout << *static_cast<Triunghi*>(pF) << endl;//afiseaza corect daca in pF retine adresa unui triunghi

    scrie(pF);


	//system("pause");
	return 1;
}
