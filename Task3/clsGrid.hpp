#pragma once
#include <iostream>
#include <memory>
#include <algorithm>
#include "WorldParams.h"

using namespace std;

template <typename T>
class clsGrid{
    protected:
        unique_ptr<T[]> _data;
        size_t _N1,_N2,_N3;
    public:
        clsGrid(){
			setDim(1, 1, 1);
        }
        clsGrid(WorldParams& wp){
			setDim(wp.grid_N.x, wp.grid_N.y, wp.grid_N.z);
			malloc();
        }

        void setDim(size_t N1,size_t N2=1,size_t N3=1){
            _N1=N1;
            _N2=N2;
            _N3=N3;
        }

		void makeZero(){
            fill(_data.get(), _data.get() + _N1*_N2*_N3, T{});
		}

        void printDim(){
            cout<<_N1<<"x"<<_N2<<"x"<<_N3<<endl;
        }
        __inline__ size_t coord2lin(size_t x,size_t y,size_t z){return z*_N2*_N1+_N1*y+x;}       
        //size_t coord2lin(size_t x,size_t y){return _N2*y+x;}
        //size_t coord2lin(size_t x){return x;}
        //https://ru.cppreference.com/w/cpp/language/inline
        
        void malloc(){
            _data= make_unique<T[]>(_N1*_N2*_N3);
        }

        T val(size_t x,size_t y=0,size_t z=0){
            return _data[coord2lin(x,y,z)];
        }

        T at_val(size_t x,size_t y=0,size_t z=0){
            if(x<_N1 && y<_N2 && z<_N3)
                return _data[coord2lin(x,y,z)];
            else   
            {
                cout<<x<<"x"<<y<<"x"<<z<<" is out of range ";
                printDim();
                cout<<"Exit."<<endl;
                exit(1);
            }
        }
        //если хотим, чтобы возвращался адрес элемента в массиве данных, то можно возвращать указатель на место в памяти данного элемента:
        //T* val(size_t x,size_t y=0,size_t z=0) {return  &_data[coord2lin(x,y,z)];}
        //и потом работать непосредственно с этим элементом:
        //*Ex.val(x,y) -- получить значение
        // (*El.val(1))=5; -- изменить
        // Если у нас элементы это вектора, то El.val(1)->x=5; -- обращение к полю икс
        //https://ru.wikipedia.org/wiki/%D0%9E%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D0%BE%D1%80%D1%8B_%D0%B2_C_%D0%B8_C%2B%2B#%D0%9E%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D0%BE%D1%80%D1%8B_%D1%80%D0%B0%D0%B1%D0%BE%D1%82%D1%8B_%D1%81_%D1%83%D0%BA%D0%B0%D0%B7%D0%B0%D1%82%D0%B5%D0%BB%D1%8F%D0%BC%D0%B8_%D0%B8_%D1%87%D0%BB%D0%B5%D0%BD%D0%B0%D0%BC%D0%B8_%D0%BA%D0%BB%D0%B0%D1%81%D1%81%D0%B0
        void set(T val,size_t x,size_t y=0,size_t z=0){
            _data[coord2lin(x,y,z)]=val;
        }  
             
        void at_set(T val,size_t x,size_t y=0,size_t z=0){
            if(x<_N1 && y<_N2 && z<_N3){
                size_t i = coord2lin(x,y,z); 
				
				#pragma omp atomic write
				_data[i]=val;
			} else {
                cout<<x<<"x"<<y<<"x"<<z<<" is out of range ";
                printDim();
                cout<<"Exit."<<endl;
                exit(1);
            }
        }       

        T* raw_pointer(){
            return _data.get();
        } 
 };
 
using grid = clsGrid<double>;
