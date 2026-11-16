#include <iostream>
#include <math.h>
#include <filesystem>
#include <format>
#include <functional>
#include <chrono>
#include <fstream>
#include <string>

#include "app/utils/clsVector.hpp"
#include "app/particle/particle.h"
#include "app/world/clsWorld.h"
#include "H5Cpp.h"

using namespace chrono;
using namespace H5;
using namespace std;
namespace fs = std::filesystem;

void create_dir(const fs::path& dirPath) {
    if (fs::exists(dirPath)) {
        // Проверяем, является ли он именно директорией (а не файлом)
        if (fs::is_directory(dirPath)) {
            cout << "Папка уже существует: " << dirPath << '\n';
        } else {
            cout << "Путь существует, но это файл: " << dirPath << '\n';
        }
    } else {
        if (fs::create_directories(dirPath)) {
            cout << "Папка успешно создана: " << dirPath << '\n';
        } else {
            cout << "Не удалось создать папку: " << dirPath << '\n';
        }
    }
}

int main() {
	double t = 0;
	clsWorld world = clsWorld();

	string folderPath = "data/";
	create_dir(folderPath);

	size_t N_max = 100;
	for (size_t i=0; i < N_max; i++){
        t = i * world.wp.dt;

		world.moveMagnetic();
		world.moveElectric();
		world.moveMagnetic();

		world.processCurrents(t);
		
		world.pushSorts();
		world.processBoundary();

		if((i%10) == 0){
			string FileName = folderPath + format("{:05}", i) + ".h5";
			H5std_string FILE_NAME(FileName);
	        H5File file(FILE_NAME, H5F_ACC_TRUNC);
			
			world.saveDensity(file);
			world.saveFields(file);
			file.close();
		}
	}
	return 0;
}