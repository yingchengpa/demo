/*import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;*/
#include <vector>
#include <deque>
#include <algorithm>
/**
 * DoubleArrayTrie在构建双数组的过程中也借助于一棵传统的Trie树，但这棵Trie树并没有被保存下来，
 * 如果要查找以prefix为前缀的所有词不适合用DoubleArrayTrie，应该用传统的Trie树。
 *
 * @author zhangchaoyang
 *
 */
template<typename String>
class DoubleArrayTrie {
private:
    //static const int BUF_SIZE = 16384;// 2^14，java采用unicode编码表示所有字符，每个字符固定用两个字节表示。考虑到每个字节的符号位都是0，所以又可以节省两个bit
    //static const int UNIT_SIZE = 8; // size of int + int
    enum {
        STRING_CHAR_BIT = sizeof(String::value_type) * 8,
        STRING_CHAR_MASK = (1 << STRING_CHAR_BIT) - 1
    };
    struct Node {
        int code;// 字符的unicode编码
        int depth;// 在Trie树中的深度
        int left;//
        int right;//
    };

    std::vector<int> check;
    std::vector<int> base;

    //std::vector<bool> used;
    int size;
    int allocSize;// base数组当前的长度
    //List<String> key;// 所有的词
    //int keySize;
    //int length[];
    //int value[];
    //int progress;
    //int nextCheckPos;
    int error_;

    // 扩充base和check数组
    int resize(int newSize, std::vector<bool>& used) {

        base.resize(newSize);
        check.resize(newSize);
        used.resize(newSize);

        return allocSize = newSize;
    }

    int fetch(const std::vector<String>& key, const Node& parent, std::vector<Node>& siblings) {
        if (error_ < 0)
            return 0;

        int prev = 0;

        for (int i = parent.left; i < parent.right; i++) {
            const String& tmp = key[i];

            if (tmp.length() < parent.depth)
                continue;

            int cur = 0;
            if (tmp.length() != parent.depth)
                cur = (unsigned int)(tmp[parent.depth] & STRING_CHAR_MASK) + 1;

            if (prev > cur) {
                error_ = -3;
                return 0;
            }

            if (cur != prev || siblings.size() == 0) {
                Node tmp_node = {};
                tmp_node.depth = parent.depth + 1;
                tmp_node.code = cur;
                tmp_node.left = i;
                if (siblings.size() != 0)
                    siblings.back().right = i;

                siblings.push_back(tmp_node);
            }

            prev = cur;
        }

        if (siblings.size() != 0)
            siblings.back().right = parent.right;

        return siblings.size();
    }

    int insert(const std::vector<String>& key, int& nextCheckPos, std::vector<bool>& used, std::vector<Node>& siblings) {
        if (error_ < 0)
            return 0;

        int begin = 0;
        int pos = (std::max)(siblings[0].code + 1, nextCheckPos) - 1;
        int nonzero_num = 0;
        int first = 0;

        while (allocSize <= pos)
            resize(allocSize * 2, used);

        while (true) {
        outer:
            pos++;

            while (allocSize <= pos)
                resize(allocSize * 2, used);

            if (check[pos] != 0) {
                nonzero_num++;
                continue;
            }
            else if (first == 0) {
                nextCheckPos = pos;
                first = 1;
            }

            begin = pos - siblings[0].code;
            while (allocSize <= (begin + siblings.back().code)) {
                // progress can be zero
                //double l = (1.05 > 1.0 * key.size() / (progress + 1)) ? 1.05 : 1.0 * key.size() / (progress + 1);
                resize(allocSize * 2, used);
            }

            if (used[begin])
                continue;

            for (int i = 1; i < siblings.size(); i++)
                if (check[begin + siblings[i].code] != 0)
                    goto outer;

            break;
        }

        // -- Simple heuristics --
        // if the percentage of non-empty contents in check between the
        // index
        // 'next_check_pos' and 'check' is greater than some constant value
        // (e.g. 0.9),
        // new 'next_check_pos' index is written by 'check'.
        if (1.0 * nonzero_num / (pos - nextCheckPos + 1) >= 0.95)
            nextCheckPos = pos;

        used[begin] = true;
        size = (std::max)(size, begin + siblings.back().code + 1);

        for (int i = 0; i < siblings.size(); i++)
            check[begin + siblings[i].code] = begin;

        for (int i = 0; i < siblings.size(); i++) {
            std::vector<Node> new_siblings;

            if (fetch(key, siblings[i], new_siblings) == 0) {
                base[begin + siblings[i].code] = -siblings[i].left - 1;

                //if (value != null && (-value[siblings[i].left] - 1) >= 0) {
                //    error_ = -2;
                //    return 0;
                //}

                //progress++;
                // if (progress_func_) (*progress_func_) (progress,
                // keySize);
            }
            else {
                int h = insert(key, nextCheckPos, used, new_siblings);
                base[begin + siblings[i].code] = h;
            }
        }
        return begin;
    }

    public:
        DoubleArrayTrie() {
        //check = null;
        //base = null;
        //used = null;
        size = 0;
        allocSize = 0;
        // no_delete_ = false;
        error_ = 0;
    }

    // no deconstructor

    // set_result omitted
    // the search methods returns (the list of) the value(s) instead
    // of (the list of) the pair(s) of value(s) and length(s)

    // set_array omitted
    // array omitted

    void clear() {
        // if (! no_delete_)
        check.clear();
        base.clear();
        used.clear();
        allocSize = 0;
        size = 0;
        // no_delete_ = false;
    }

    //int getUnitSize() {
    //    return UNIT_SIZE;
    //}

    int getSize() {
        return size;
    }

    //int getTotalSize() {
    //    return size * UNIT_SIZE;
    //}

    int getNonzeroSize() {
        int result = 0;
        for (int i = 0; i < size; i++)
            if (check[i] != 0)
                result++;
        return result;
    }

    int build(const std::vector<String>& key) {

        // progress_func_ = progress_func;
        //key = _key;
        //length = _length;
        //keySize = key.size();
        //value = _value;
        //progress = 0;

        std::vector<bool> used; 
        resize(1 << (STRING_CHAR_BIT + 1), used);

        base[0] = 1;

        Node root_node = {};
        root_node.left = 0;
        root_node.right = key.size();
        root_node.depth = 0;

        std::vector<Node> siblings;
        fetch(key, root_node, siblings);
        int nextCheckPos = 0;
        insert(key, nextCheckPos, used, siblings);

        // size += (1 << 8 * 2) + 1; // ???
        // if (size >= allocSize) resize (size);

        //used.clear();
        //key = null;

        return error_;
    }

    int exactMatchSearch(const String& key) {
        int pos = 0;
        int len = key.length();
        int nodePos = 0;

        int result = -1;

        int b = base[nodePos];
        int p;

        for (int i = pos; i < len; i++) {
            p = b + (unsigned int)(key[i] & STRING_CHAR_MASK) + 1;
            if (b == check[p])
                b = base[p];
            else
                return result;
        }

        p = b;
        int n = base[p];
        if (b == check[p] && n < 0) {
            result = -n - 1;
        }
        return result;
    }

    std::vector<int> commonPrefixSearch(const String& key) {
        int pos = 0;
        int len = key.length();
        int nodePos = 0;

        std::vector<int> result;

        int b = base[nodePos];
        int n;
        int p;

        for (int i = pos; i < len; i++) {
            p = b;
            n = base[p];

            if (b == check[p] && n < 0) {
                result.push_back(-n - 1);
            }

            p = b + (unsigned int)(key[i] & STRING_CHAR_MASK) + 1;
            if (b == check[p])
                b = base[p];
            else
                return result;
        }

        p = b;
        n = base[p];

        if (b == check[p] && n < 0) {
            result.push_back(-n - 1);
        }

        return result;
    }
};