#include<iostream>
#include<bitset>
#include<string>
#include<fstream>
#include<algorithm>
#include<chrono>

#define MAX_SIZE 513

using namespace std;

//string input_file_name = "test_19.inp";
//string output_file_name = "test_00.out";

bitset<4> hex_char_to_bin(char c);
bitset<MAX_SIZE> hex_str_to_bin(const string& hex);
bitset<MAX_SIZE> subtract_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
bitset<MAX_SIZE> add_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
int compare_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
string binary_to_hex_little_endian(const bitset<MAX_SIZE>& binary);
bitset<MAX_SIZE> power_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> mul_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& y, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> modulo(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& n);

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

    if (!input_file.is_open())
    {
        cout << "Can't open file " << argv[1] << endl;
        return 1;
    }

    string p_hex, g_hex, a_hex, b_hex;
    getline(input_file, p_hex, '\n');
    getline(input_file, g_hex, '\n');
    getline(input_file, a_hex, '\n');
    getline(input_file, b_hex, '\n');

    reverse(p_hex.begin(), p_hex.end());
    reverse(g_hex.begin(), g_hex.end());
    reverse(a_hex.begin(), a_hex.end());
    reverse(b_hex.begin(), b_hex.end());

    bitset<MAX_SIZE> p = hex_str_to_bin(p_hex);
    bitset<MAX_SIZE> g = hex_str_to_bin(g_hex);
    bitset<MAX_SIZE> a = hex_str_to_bin(a_hex);
    bitset<MAX_SIZE> b = hex_str_to_bin(b_hex);

    ofstream output_file(argv[2]);
    //ofstream output_file(output_file_name);

    if (!output_file.is_open())
    {
        cout << "Can't open file " << argv[2] << endl;
        return 1;
    }
    bitset<MAX_SIZE> public_alice = power_mod(g, a, p); // g^a (mod p)
    bitset<MAX_SIZE> public_bob = power_mod(g, b, p); // g^b (mod p)
    bitset<MAX_SIZE> common_public_key = power_mod(public_alice, b, p); // g^(ab) (mod p)
    
    string public_alice_str = binary_to_hex_little_endian(public_alice);
    string public_bob_str = binary_to_hex_little_endian(public_bob);
    string common_public_key_str = binary_to_hex_little_endian(common_public_key);
    
    /*cout << "g^a mod p: " << public_alice_str << endl;
    cout << "g^b mod p: " << public_bob_str << endl;
    cout << "g^ab mod p: " << common_public_key_str << endl;
    */
    
    output_file << public_alice_str << endl;
    output_file << public_bob_str << endl;
    output_file << common_public_key_str << endl;

    output_file.close();

    /*auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    cout << "Time: " << duration << " s" << endl;*/
    
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

string binary_to_hex_little_endian(const bitset<MAX_SIZE>& binary)
{
    string bin;
    for (int i = 0; i < MAX_SIZE - 1; ++i)
        bin += (binary[i] + 48);

    int last_one_index = -1;
    for (int i = bin.size() - 1; i >= 0; --i)
    {
        if (bin[i] == '1')
        {
            last_one_index = i;
            break;
        }
    }

    if (last_one_index == -1) return "0";

    string result = bin.substr(0, last_one_index + 1);

    while (result.size() % 4 != 0)
    {
        result = result + "0";
    }

    reverse(result.begin(), result.end());

    string rest, tmp;

    for (int i = 0; i < result.length(); i += 4)
    {
        tmp = result.substr(i, 4);
        if (!tmp.compare("0000"))
        {
            rest = "0" + rest;
        }
        else if (!tmp.compare("0001"))
        {
            rest = "1" + rest;
        }
        else if (!tmp.compare("0010"))
        {
            rest = "2" + rest;
        }
        else if (!tmp.compare("0011"))
        {
            rest = "3" + rest;
        }
        else if (!tmp.compare("0100"))
        {
            rest = "4" + rest;
        }
        else if (!tmp.compare("0101"))
        {
            rest = "5" + rest;
        }
        else if (!tmp.compare("0110"))
        {
            rest = "6" + rest;
        }
        else if (!tmp.compare("0111"))
        {
            rest = "7" + rest;
        }
        else if (!tmp.compare("1000"))
        {
            rest = "8" + rest;
        }
        else if (!tmp.compare("1001"))
        {
            rest = "9" + rest;
        }
        else if (!tmp.compare("1010"))
        {
            rest = "A" + rest;
        }
        else if (!tmp.compare("1011"))
        {
            rest = "B" + rest;
        }
        else if (!tmp.compare("1100"))
        {
            rest = "C" + rest;
        }
        else if (!tmp.compare("1101"))
        {
            rest = "D" + rest;
        }
        else if (!tmp.compare("1110"))
        {
            rest = "E" + rest;
        }
        else if (!tmp.compare("1111"))
        {
            rest = "F" + rest;
        }
        else
        {
            continue;
        }
    }
    return rest;
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