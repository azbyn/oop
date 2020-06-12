#pragma once

#include<iostream>
#define PI 3.14159
using namespace std;

class Punct{
	char c;
	int x, y;
public:
	Punct(int x = 0, int y = 0,char c='A');
	Punct(const Punct&);
	int getX() const{ return x; }
	void setX(int x) { this->x = x; }
	int getY() const{ return y; }
	void setY(int y) { this->y = y; }
	char getText() const{ return c; }
	void setText(char c) { this->c = c; }
	static double distanta(const Punct &, const Punct&);
	int operator ==(const Punct&);
	int operator !=(const Punct&);
	friend ostream& operator<<(ostream &, const Punct&);
	friend istream& operator>>(istream &, Punct &);
};

class Figura{
protected:
	int n;
	Punct* P;
public:

	Figura(const int,const Punct*);
	Figura(const Figura&);
	~Figura();
	Figura operator+(const Punct&);//supraincarcare operator + pt adaugare punct
	Figura operator-(const Punct&);//supraincarcare operator - pt eliminare punct
	Figura& operator =(const Figura&);
	virtual double Perimetru();//metoda virtuala
	virtual double Aria();//metoda virtuala
	virtual int InInterior(const Punct &);
	friend ostream& operator<<(ostream &, const Figura&);
};

class Dreptunghi:public Figura{
public:
	Dreptunghi(const Punct&,const Punct&);//stanga sus si dreapta jos
	double Aria();
	friend ostream& operator<<(ostream &, const Dreptunghi&);
};
class Cerc :public Figura
{
	double r;
public:
	Cerc(const Punct&, const double&);
	double Perimetru();
	double Aria();
	int InInterior(const Punct &);
	operator Dreptunghi();//cast din cerc in dreptunghi(patrat)
};
class Triunghi :public Figura
{
public:
	Triunghi(const Punct&, const Punct&, const Punct&);
	friend ostream& operator<<(ostream &, const Triunghi&);
};
