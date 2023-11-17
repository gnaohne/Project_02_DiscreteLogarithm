#include<iostream>
#include<bitset>
#include<fstream>
#include<algorithm>
#include<string>
#include<chrono>
#include<vector>

#define MAX_SIZE 513

using namespace std;

//string input_file_name = "test_19.inp";
//string output_file_name = "test_00.out";

bitset<4> hex_char_to_bin(char c);
bitset<MAX_SIZE> hex_str_to_bin(const string& hex);
bitset<MAX_SIZE> subtract_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
bitset<MAX_SIZE> add_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
int compare_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
bitset<MAX_SIZE> power_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> mul_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& y, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> modulo(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> div_binary(const bitset<MAX_SIZE>& dividend, const bitset<MAX_SIZE>& divisor);

int main(int argc, char* argv[])
{
    //auto start = std::chrono::high_resolution_clock::now();

    if (argc != 3)
    {
        cerr << "Usage: $ ./main test_XX.inp test_XX.out" << endl;
        return 1;
    }

    ifstream input_file(argv[1]);

    //ifstream input_file(input_file_name);

    string p_hex, g_hex, str_n;
    vector<string> divisor_hex;
    int n;

    if (!input_file.is_open())
    {
        cout << "Can't open file " << argv[1] << endl;
        return 1;
    }

    getline(input_file, p_hex, '\n');
    reverse(p_hex.begin(), p_hex.end());
    //cout << "p_hex: " << p_hex << endl;

    getline(input_file, str_n, '\n');
    reverse(str_n.begin(), str_n.end());
    //cout << "n_hex: " << str_n << endl;

    n = stoi(str_n, nullptr, 16);
    //cout << "n: " << n << endl;

    divisor_hex.resize(n);
    
    for (int i = 0; i < n; ++i)
    {
        input_file >> divisor_hex[i];
        reverse(divisor_hex[i].begin(), divisor_hex[i].end());
        //cout << "divisor_hex: " << divisor_hex[i] << endl;
    }

    getline(input_file, g_hex, '\n');
    getline(input_file, g_hex, '\n');
    reverse(g_hex.begin(), g_hex.end());
    //cout << "g_hex: " << g_hex << endl;
    input_file.close();

    bitset<MAX_SIZE> p = hex_str_to_bin(p_hex);
    //cout << "p: " << p << endl;
    //cout << "p: " << p.to_ullong() << endl;

    bitset<MAX_SIZE> p_1 = subtract_binary(p, bitset<MAX_SIZE>(1));
    //cout << "p-1: " << p_1.to_ullong() << endl;
    bitset<MAX_SIZE> g = hex_str_to_bin(g_hex);
    //cout << "g: " << g << endl;
    //cout << "g: " << g.to_ullong() << endl;

    vector<bitset<MAX_SIZE>> divisor;
    divisor.resize(n);
    for (int i = 0; i < n; i++)
    {
        divisor[i] = div_binary(p_1, hex_str_to_bin(divisor_hex[i]));
        //cout << "divisor: " << divisor[i].to_ullong() << endl;
    }

    int flag = 1;
    for (int i = 0; i < n; ++i)
    {
        bitset<MAX_SIZE> result = power_mod(g, divisor[i], p);
        if (compare_binary(result, bitset<MAX_SIZE>(1)) == 0)
        {
            flag = 0;
            break;
        }
    }

    ofstream output_file(argv[2]);
    //ofstream output_file(output_file_name);

    if (!output_file.is_open())
    {
        cout << "Can't open file " << argv[2] << endl;
        return 1;
    }
    
    output_file << flag;
    //cout << flag << endl;
    output_file.close();

    /*auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    cout << "Time: " << duration << " s" << endl;
	*/

    return 0;
}

bitset<4> hex_char_to_bin(char c)
{
    switch (toupper(c))
    {
    case '0': return bitset<4>("0000");
    case '1': return bitset<4>("0001");
    case '2': return bitset<4>("0010");
    case '3': return bitset<4>("0011");
    case '4': return bitset<4>("0100");
    case '5': return bitset<4>("0101");
    case '6': return bitset<4>("0110");
    case '7': return bitset<4>("0111");
    case '8': return bitset<4>("1000");
    case '9': return bitset<4>("1001");
    case 'A': return bitset<4>("1010");
    case 'B': return bitset<4>("1011");
    case 'C': return bitset<4>("1100");
    case 'D': return bitset<4>("1101");
    case 'E': return bitset<4>("1110");
    }
    return bitset<4>("1111");
}

bitset<MAX_SIZE> hex_str_to_bin(const string& hex)
{
    bitset<MAX_SIZE> bin;
    int length = hex.length() * 4;
    int bitset_size = min(length, MAX_SIZE);

    for (int i = 0; i < bitset_size; ++i)
    {
        bin <<= 1;
        char hex_char = hex[i / 4];
        int hex_value = hex_char_to_bin(hex_char).to_ulong();
        int shift = 3 - (i % 4);
        int bit = (hex_value >> shift) & 1;
        bin |= bit;
    }

    return bin;
}

bitset<MAX_SIZE> add_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b)
{
    bitset<MAX_SIZE> result = a;
    result ^= b;
    bitset<MAX_SIZE> carry = (a & b) << 1;
    while (carry != 0)
    {
        bitset<MAX_SIZE> temp_result = result;
        result ^= carry;
        carry = (temp_result & carry) << 1;
    }
    return result;
}

int compare_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b)
{
    for (int i = MAX_SIZE - 2; i >= 0; --i)
    {
        if (a[i] < b[i])
        {
            return -1; // a < b
        }
        else if (a[i] > b[i])
        {
            return 1; // a > b
        }
    }
    return 0; // a = b
}


bitset<MAX_SIZE> subtract_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b)
{

    bitset<MAX_SIZE> result = a;
    result ^= b;
    bitset<MAX_SIZE> borrow = ((~a) & b) << 1;
    while (borrow != 0)
    {
        bitset<MAX_SIZE> temp_result = result;
        result ^= borrow;
        borrow = ((~temp_result) & borrow) << 1;
    }
    return result;
}

bitset<MAX_SIZE> modulo(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& n)
{

    bool compare = compare_binary(x, n);
    if (compare == 0) return bitset<MAX_SIZE>(0);
    if (compare == -1) return x;

    bitset<MAX_SIZE> adjusted_x = x;
    bitset<MAX_SIZE> adjusted_n = n;

    while (compare_binary(adjusted_x, adjusted_n) == 1 || compare_binary(adjusted_x, adjusted_n) == 0)
    {
        adjusted_x = subtract_binary(adjusted_x, adjusted_n);
    }

    return adjusted_x;
}

bitset<MAX_SIZE> mul_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& y, const bitset<MAX_SIZE>& n)
{
    bitset<MAX_SIZE> temp_x = modulo(x, n);
    bitset<MAX_SIZE> result(0);

    int count = 0;
    for (int i = MAX_SIZE - 1; i >= 0; --i)
    {
        if (y[i] == 1) break;
        else
        {
            count++;
        }
    }

    if (y[0] == 1)
    {
        result = temp_x;
    }

    for (int i = 1; i < MAX_SIZE - count; ++i)
    {
        temp_x <<= 1; // temp_x = temp_x * 2
        temp_x = modulo(temp_x, n);

        if (y[i] == 1) {
            result = add_binary(result, temp_x);
            result = modulo(result, n);
        }
    }
    return result;
}

bitset<MAX_SIZE> power_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& n)
{

    bitset<MAX_SIZE> result(1);

    bool start = false;

    for (int i = a.size() - 1; i >= 0; --i)
    {
        if (a[i] == 1)start = true;

        if (start)
        {
            // y = y^2 (mod n)
            result = mul_mod(result, result, n);

            if (a[i] == 1)
            {
                result = mul_mod(result, x, n); // y = y*x (mod n)
            }
        }
        else continue;
    }

    return result;
}

bitset<MAX_SIZE> div_binary(const bitset<MAX_SIZE>& dividend, const bitset<MAX_SIZE>& divisor)
{
    bitset<MAX_SIZE> quotient = 0;
    bitset<MAX_SIZE> remainder = dividend;
    int dividend_count = 0, divisor_count = 0;
    bool flag1 = false, flag2 = false;
    for (int i = MAX_SIZE - 2; i >= 0; --i) // 1 bit sign 
    {
        if (flag1 && flag2) break;
        
        if (dividend[i] == 0 && !flag1) dividend_count++;
        else flag1 = true;

        if (divisor[i] == 0 && !flag2) divisor_count++;
        else flag2 = true;
    }

    int difference = divisor_count - dividend_count; // = (MAX_SIZE - divident_count) - (MAX_SIZE - divisor_count)

    bitset<MAX_SIZE> divisor_temp = divisor;
    divisor_temp <<= difference;
    
    for (int i = 0; i <= difference; ++i) 
    {
        if (compare_binary(remainder, divisor_temp) != -1)
        {
            remainder = subtract_binary(remainder, divisor_temp);
            quotient.set(difference - i);
        }
        divisor_temp >>= 1;
    }
    return quotient;
}