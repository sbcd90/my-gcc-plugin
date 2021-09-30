struct A {
    int *addr;
    ~A();
};

A build_A();

int main() {
    build_A();
    return 0;
}