pipeline {
	agent {
		label 'master'
	}
	stages {
		stage('Code style check') {
			steps {
				script {
					final filesToCheck = sh(script: "find firmware -type f -not -path 'firmware/lib*' -not -path 'firmware/build*' \\( -name '*.h' -or -name '*.hpp' -or -name '*.c' -or -name '*.cpp' \\)", returnStdout: true).split()
					for (file in filesToCheck) {
						echo "Checking ${file}..."
						if (sh(script: "clang-format ${file} --dry-run --Werror", returnStatus: true)) {
							sh script: "clang-format ${file} | git diff --no-index -- ${file} -", returnStatus: true
							error "${file} was not formatted!"
						}
					}
				}
			}
		}
		stage('Generate build files') {
			steps {
				dir(path: './firmware') {
					sh 'cmake -G Ninja -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Debug -DQEMU=OFF'
				}
			}
		}
		stage('Build: Bootloader') {
			steps {
				dir(path: './firmware') {
					sh 'cmake --build build --target bootloader'
					archiveArtifacts artifacts: 'build/bootloader/bootloader.hex', onlyIfSuccessful: true
				}
			}
		}
		stage('Build: Firmware') {
			steps {
				dir(path: './firmware') {
					sh 'cmake --build build --target application'
					archiveArtifacts artifacts: 'build/application/application.hex', onlyIfSuccessful: true
				}
			}
		}
	}
	post {
		always {
			cleanWs()
		}
	}
}