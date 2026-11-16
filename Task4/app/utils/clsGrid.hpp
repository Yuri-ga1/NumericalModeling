#pragma once
#include <iostream>
#include <memory>
#include <algorithm>
#include <string>

#include "app/world/WorldParams.h"
#include "app/utils/clsVector.hpp"

#include "H5Cpp.h"

using namespace std;
using namespace H5;

template <typename T>
class clsGrid{
    protected:
        unique_ptr<T[]> _data;
        size_t _N1,_N2,_N3;
		WorldParams wp;
    public:
		string name = "None";

        clsGrid(){
			setDim(1, 1, 1); wp = WorldParams();
        }

        clsGrid(string _name){
			wp = WorldParams();
			setDim(wp.grid_N.x, wp.grid_N.y, wp.grid_N.z);
			name = _name;
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

			vec c = vec(x,y,z);
			vec c_new = vec(x,y,z);
			vec bounds = vec(_N1,_N2,_N3);
			
			for (size_t i=0; i<3; i++){
				if (wp.btype[i] == Boundary::Reflective){
					if (c.at(i) >= bounds.at(i) || c.at(i) <0) return 0;
				} else {
					if (c.at(i) >= bounds.at(i)) c_new.set(i, c.at(i) - bounds.at(i));
					if (c.at(i) < 0) c_new.set(i, bounds.at(i) + c.at(i));
				}
			}

			x = c_new.x;
			y = c_new.y;
			z = c_new.z;
			
			return _data[coord2lin(x,y,z)];
        }

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

		void add(T val,size_t x,size_t y=0,size_t z=0){
			T old_val = at_val(x,y,z);
			at_set(val + old_val, x,y,z);
		}

		void save2h5(H5File& fp){
			try{
				H5std_string DATASET_NAME(name);
				hsize_t dimsf[2]; // dataset dimensions
				
		        dimsf[0] = _N1;
		        dimsf[1] = _N2;
				
		        DataSpace dataspace(2, dimsf);
		        FloatType datatype(PredType::NATIVE_DOUBLE);
		        datatype.setOrder(H5T_ORDER_LE);
				
				DataSet dataset = fp.createDataSet(DATASET_NAME, datatype, dataspace);
		        dataset.write(raw_pointer(), PredType::NATIVE_DOUBLE);

				dataspace.close();
				dataset.close();
			} catch (FileIException error) {
		        error.printErrorStack();
		    }
			catch (DataSetIException error) {
		        error.printErrorStack();
		    }
			catch (DataSpaceIException error) {
		        error.printErrorStack();
		    }
		    catch (DataTypeIException error) {
		        error.printErrorStack();
		    }
		}
 };
 
using grid = clsGrid<double>;
