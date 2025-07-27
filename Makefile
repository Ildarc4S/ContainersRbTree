IMAGE_NAME = s21containers
DOCKERFILE = docker/Dockerfile

.PHONY: docker-build
docker-build:
	docker build -t $(IMAGE_NAME) -f $(DOCKERFILE) .

.PHONY: build
build:
	docker run --rm -v $(PWD):/project -w /project/build $(IMAGE_NAME) bash -c "cmake -DCOVERAGE=ON -B . -S .. && cmake --build ."

.PHONY: test
test:
	docker run --rm -it -v $(PWD):/project -w /project/build $(IMAGE_NAME) ctest --output-on-failure

.PHONY: test-%
test-%:
	docker run --rm -it -v $(PWD):/project -w /project/build $(IMAGE_NAME) ctest -R ^$*$$ --output-on-failure

.PHONY: test-verbose-%
test-verbose-%:
	docker run --rm -v $(PWD):/project -w /project/build $(IMAGE_NAME) ctest -V -R ^$*$$

.PHONY: cppcheck
cppcheck:
	docker run --rm -v $(PWD):/project -w /project/build $(IMAGE_NAME) cmake --build . --target cppcheck-test

.PHONY: cppcheck-sources
cppcheck-sources:
	docker run --rm -v $(PWD):/project -w /project/build $(IMAGE_NAME) cmake --build . --target cppcheck-test-sources

.PHONY: clang-tidy
clang-tidy:
	docker run --rm -v $(PWD):/project -w /project/build $(IMAGE_NAME) cmake --build . --target clang-tidy-test

.PHONY: clang-format
clang-format:
	docker run --rm -v $(PWD):/project -w /project/build $(IMAGE_NAME) cmake --build . --target clang-format-test

.PHONY: clang-format-fix
clang-format-fix:
	docker run --rm -v $(PWD):/project -w /project/build $(IMAGE_NAME) cmake --build . --target clang-format-fix

.PHONY: valgrind
valgrind:
	docker run --rm -v $(PWD):/project -w /project/build $(IMAGE_NAME) cmake --build . --target valgrind-test

.PHONY: valgrind-%
valgrind-%:
	docker run --rm -v $(PWD):/project -w /project/build $(IMAGE_NAME) cmake --build . --target valgrind_$*

.PHONY: coverage
coverage:
	docker run --rm -v $(PWD):/project -w /project/build $(IMAGE_NAME) cmake --build . --target coverage_all

.PHONY: help
help:
	@echo 'make docker-build           — собрать docker image'
	@echo 'make build                  — пересобрать проект в контейнере'
	@echo 'make test                   — все тесты (ctest)'
	@echo 'make test-<имя_теста>       — один тест (make test-s21_test_rb_tree)'
	@echo 'make test-verbose-<имя>     — один тест подробно'
	@echo 'make cppcheck               — static analysis (cppcheck)'
	@echo 'make clang-tidy             — static analysis (clang-tidy)'
	@echo 'make clang-format           — check code style'
	@echo 'make clang-format-fix       — автоисправить стиль'
	@echo 'make valgrind               — все тесты с valgrind'
	@echo 'make valgrind-<имя_теста>   — один тест с valgrind'
	@echo 'make coverage               — code coverage с генерацией html'

.PHONY: clean
clean:
	docker run --rm -v $(PWD):/project -w /project $(IMAGE_NAME) rm -rf build

