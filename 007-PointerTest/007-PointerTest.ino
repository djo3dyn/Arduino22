
int global;

void setup()
{
    Serial.begin(9600);

}

void loop()
{
    Serial.print("Global Var : ");
    Serial.println(global);
    Serial.print("RAM Addr : ");
    Serial.println((int)&global , 16);
    Serial.println();

    int local = 126;
    Serial.print("Local Var : ");
    Serial.println(local);
    Serial.print("RAM Addr : ");
    Serial.println((int)&local , 16);
    Serial.println();

    func();

    global++;
    delay(1000);

}

void func()
{
    int local = 1001;
    Serial.print("Local on function Var : ");
    Serial.println(local);
    Serial.print("RAM Addr : ");
    Serial.println((int)&local , 16);
    Serial.println();

}