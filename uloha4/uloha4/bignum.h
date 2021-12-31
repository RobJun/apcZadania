#pragma once

// here you can include whatever you want :)
#include <iostream>
#include <string>
#include <stdint.h>
#include <algorithm>

#define SUPPORT_DIVISION 1 // define as 1 when you have implemented the division
#define SUPPORT_IFSTREAM 1 // define as 1 when you have implemented the input >>

// if you do not plan to implement bonus, just delete those lines
class BigNum final
{
public:
    // constructors
    BigNum() : m_number("\0"), m_negative(false) {};
    BigNum(int64_t n) : m_number(std::to_string((n < 0 ? (n * -1) : n))), m_negative((n < 0) ? true : false) {
        for (auto& c : m_number) {
            c -= '0';
        }
    };
    explicit BigNum(const std::string& str)
        : m_negative(false) {
        if (str.empty() ||str.find_first_not_of("+-0123456789") != str.npos) {
            throw std::exception();
        }
        size_t oper = str.find_first_of("+-");
        if (oper != str.npos) {
            if (oper != 0) throw std::exception();
            if (str.find_first_of("+-", oper + 1) != str.npos) throw std::exception();
            if (str.substr(1, str.length() - 1).length() == 0) throw std::exception();
            m_number = deleteZeroes(str.substr(1, str.length() - 1));
            if (str[oper] == '-') {
                if (m_number == "0") m_negative = false;
                else m_negative = true;
            }
        }
        else {
            m_number = deleteZeroes(str);
        }

        for (auto& num : m_number) {
            num = num - '0';
        }
    };

    // copy
    BigNum(const BigNum& other) = default;
    BigNum& operator=(const BigNum& rhs) = default;

    // unary operators
    const BigNum& operator+() const {
        return *this;
    };
    BigNum operator-() const {
        BigNum copy = *this;
        if (copy.strigify() == "0") return copy;
        copy.m_negative = !copy.m_negative;
        return copy;
    };

    // binary arithmetics operators
    BigNum& operator+=(const BigNum& rhs) {
        if (areBothSameOper(rhs))
            return add(rhs);
        return sub(rhs, false);
    };
    BigNum& operator-=(const BigNum& rhs) {
        if (areBothSameOper(rhs))
            return sub(rhs, true);
        return add(rhs);
    }
    BigNum& operator*=(const BigNum& rhs) {
        return mul(rhs);
    }

#if SUPPORT_DIVISION == 1
    BigNum& operator/=(const BigNum& rhs) {
        auto divide = div(rhs);
        m_number = divide.first;
        makeCalculatable();
        return *this;
    }// bonus
    BigNum& operator%=(const BigNum& rhs) {
        auto divide = div(rhs);
        m_number = divide.second;
        makeCalculatable();
        return *this;
    }// bonus
#endif



private:
    // here you can add private data and members, but do not add stuff to 
    // public interface, also you can declare friends here if you want
    std::string m_number;
    bool m_negative;

    friend bool operator==(const BigNum& lhs, const BigNum& rhs);
    friend bool operator!=(const BigNum & lhs, const BigNum & rhs);
    friend bool operator<(const BigNum & lhs, const BigNum & rhs);
    friend bool operator>(const BigNum & lhs, const BigNum & rhs);
    friend bool operator<=(const BigNum & lhs, const BigNum & rhs);
    friend bool operator>=(const BigNum & lhs, const BigNum & rhs);
    friend std::ostream& operator<<(std::ostream& lhs, const BigNum& rhs);
    friend std::strong_ordering operator<=>(const BigNum& lhs, const BigNum& rhs);


    bool compareNums(const BigNum& rhs) {
        std::string left = strigify(), right = rhs.strigify();
        while (left.size() < right.size())
            left = '0' + left;
        while (right.size() < left.size())
            right = '0' + right;

        return right < left;
    }
    void makeCalculatable() {
        for (auto& i : m_number) {
            i -= '0';
        }
    }

    std::string deleteZeroes(const std::string& str) {
        auto firstNonZero = str.find_first_not_of("0");
        if (firstNonZero == str.npos) return "0";
        return str.substr(firstNonZero, str.size() - firstNonZero);
    }

    bool areBothSameOper(const BigNum& rhs) {
        if ((m_negative && rhs.m_negative) || (!m_negative && !rhs.m_negative))
            return true;
        else
            return false;
    }
    std::pair<bool,std::pair<std::string, std::string>> whichIsBigger(const BigNum& rhs) {
        if (compareNums(rhs)) {
            return { false , {m_number,rhs.m_number } };
        }
        else {
            return { true, {rhs.m_number,m_number} };
        }

    }

    std::string strigify() const {
        std::string res;
        for (auto num : m_number) {
            res += num + '0';
        }
        return res;
    }


    std::string sum(std::string& result, std::string& rhs) {
        size_t i = 0;
        char remainder = 0;
        while (result.size() < rhs.size()) result += (char)0;
        while (rhs.size() < result.size()) rhs += (char)0;
        for (; i < rhs.size(); i++) {
            char n = result[i] + rhs[i] + remainder;
            remainder = n / ((char)10);
            result[i] = n % 10;
        }

        return result;
    }
    void sub(std::string& lhs, std::string& rhs) {
        std::string r = rhs;
        char remainder = 0;
        std::reverse(lhs.begin(), lhs.end());
        std::reverse(rhs.begin(), rhs.end());
        while (lhs.size() < rhs.size()) lhs += (char)0;
        while (rhs.size() < lhs.size()) rhs += (char)0;
        for (size_t i = 0; i < rhs.size(); i++) {
            if (lhs[i] < rhs[i]) {
                remainder = 10;
                lhs[i + 1] -= 1;
            }
            lhs[i] = lhs[i] + remainder - rhs[i];
            remainder = 0;
        }
        std::reverse(lhs.begin(), lhs.end());

    }


    BigNum& add(const BigNum& rhs) {
        auto l = whichIsBigger(rhs);
        std::pair<std::string,std::string> nums = l.second;
        nums.first = ((char)0) + nums.first;
        std::reverse(nums.first.begin(), nums.first.end());
        std::reverse(nums.second.begin(), nums.second.end());
        nums.first = sum(nums.first, nums.second);
        std::reverse(nums.first.begin(), nums.first.end());
        m_number = (nums.first[0] != 0) ? nums.first : nums.first.substr(1,nums.first.size() - 1);
    
        return *this;
    }
    BigNum& sub(const BigNum& rhs, bool rightNegate) {
        auto l = whichIsBigger(rhs);
        std::pair<std::string, std::string> nums = l.second;
        sub(nums.first, nums.second);
        for (auto& i : nums.first) {
            i += '0';
        }
        std::string m = m_number = deleteZeroes(nums.first);

        for (auto& i : m_number) {
            i -= '0';
        }

        if (m == "0") {
            m_negative = false;
        }
        else if (rightNegate && l.first) {
            m_negative = true;

        } 
        else if (rightNegate && !l.first) {
            m_negative = false;
        }
        else if (l.first){
            m_negative = rhs.m_negative;
        }

        return *this;
    }

    BigNum& mul(const BigNum& rhs) {

        if (areBothSameOper(rhs))
            m_negative = false;
        else
            m_negative = true;
        std::string first = m_number, second = rhs.m_number;
        std::reverse(first.begin(), first.end());
        std::reverse(second.begin(), second.end());
        std::string row(first.size()+second.size() + 1,0);
        std::string res(first.size() + second.size() + 1, 0);
        for (size_t i = 0; i < second.size(); i++) {
            for (size_t j = 0; j <= i; j++) {
                row[j] = (char)0;
            }
            for (size_t j = 0; j < first.size(); j++) {
                char n = first[j] * second[i] + row[i+j];
                row[i+j+1] = n / 10;
                row[i + j] = n % 10;
                
            }
            res = sum(res, row);
        }


        std::reverse(res.begin(), res.end());

        for (auto& i : res) {
            i += '0';
        }

        res = deleteZeroes(res);

        if (res == "0") {
            m_negative = false;
        }

        for (auto& i : res) {
            i -= '0';
        }
        m_number = res;

        return *this;
    }


    //vracia vysledok a zvysok
    std::pair<std::string, std::string> div(const BigNum& rhs) {
        if (rhs.strigify() == "0") throw "Division by zero";
        if (areBothSameOper(rhs))
            m_negative = false;
        else
            m_negative = true;

        
        if (rhs.m_number.size() > m_number.size())
            return { "0",strigify()};

        BigNum rem; //zvysok
        //aby bol delitel kladny
        BigNum dem = rhs;
        dem.m_negative = false;

        std::string q;
        rem.m_number = "";
        for (size_t i = 0; i < m_number.size(); i++) {
            if (rem.m_number.size() == 1 && rem.m_number[0]+'0' == '0') rem.m_number = m_number[i];
            else rem.m_number += m_number[i];
            uint64_t pocet = 0;
            while(rem >= dem){
                rem -= dem;
                pocet++;
            }
            q += std::to_string(pocet)[0];
        }

        size_t index = q.find_first_not_of("0");
        if (index == q.npos) q = "0";
        else {
            q = q.substr(index, q.size() - index);
            if (q.empty())
                q = "0";
        }
        if (q == "0") {
            m_negative = false;
        }

        rem.m_number = deleteZeroes(rem.strigify());
        for (auto& i : rem.m_number) {
            i -= '0';
        }

        return { q , rem.strigify() };
    }


};

BigNum operator+(BigNum lhs, const BigNum& rhs) {
    lhs += rhs;
    return lhs;
};
BigNum operator-(BigNum lhs, const BigNum& rhs) {
    lhs -= rhs;
    return lhs;
};
BigNum operator*(BigNum lhs, const BigNum& rhs) {
    lhs *= rhs;
    return lhs;
};

#if SUPPORT_DIVISION == 1
BigNum operator/(BigNum lhs, const BigNum& rhs) {
    lhs /= rhs;
    return lhs;
}// bonus
BigNum operator%(BigNum lhs, const BigNum& rhs) {
    lhs %= rhs;
    return lhs;
}// bonus
#endif

// alternatively you can implement
std::strong_ordering operator<=>(const BigNum& lhs, const BigNum& rhs) {
    std::string left = lhs.strigify(), right = rhs.strigify();
    while (left.size() < right.size())
        left = '0' + left;
    while (right.size() < left.size())
        right = '0' + right;

    int compare = left.compare(right);

    if ((lhs.m_negative && !rhs.m_negative) ||
        (lhs.m_negative && rhs.m_negative && compare > 0) ||
        (!lhs.m_negative && !rhs.m_negative && compare < 0))
        return std::strong_ordering::less;
    else if ((!lhs.m_negative && rhs.m_negative) ||
        (lhs.m_negative && rhs.m_negative && compare < 0) ||
        (!lhs.m_negative && !rhs.m_negative && compare > 0))
        return std::strong_ordering::greater;
    
    return std::strong_ordering::equivalent;
}
// idea is, that all comparison should work, it is not important how you do it

bool operator==(const BigNum& lhs, const BigNum& rhs) = default;
bool operator!=(const BigNum& lhs, const BigNum& rhs) = default;
bool operator<(const BigNum & lhs, const BigNum & rhs) = default;
bool operator>(const BigNum & lhs, const BigNum & rhs) = default;
bool operator<=(const BigNum & lhs, const BigNum & rhs) = default;
bool operator>=(const BigNum & lhs, const BigNum & rhs) = default;




std::ostream& operator<<(std::ostream& lhs, const BigNum& rhs) {
    lhs << ((rhs.m_negative) ? "-" : "") << rhs.strigify();
    return lhs;
};

#if SUPPORT_IFSTREAM == 1
std::istream& operator>>(std::istream& lhs, BigNum& rhs) {
    try {
        std::string tmp;
        lhs >> tmp;
        if (tmp == "-") throw std::exception();
        rhs = BigNum(tmp);
    }
    catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
        lhs.setstate(std::ios_base::failbit);
        rhs = BigNum{};
    }
    return lhs;
}// bonus
#endif
