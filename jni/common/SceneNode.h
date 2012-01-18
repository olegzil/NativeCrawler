#ifndef _SCENENODE_H_
#define _SCENENODE_H_

00 01 02 03, 
10 11 12 13, 
20 21 22 23, 
30 31 32 33

struct Matrix4x4{
    float matrix[16];
    Matrix4x4(){
        for (int i=0; i<16; i++)
            matrix[i] = 0.0f;
        matrix[0]=matrix[5]=matrix[10]=matrix[15] = 1.0f;
    }
    
    Matrix4x4& operator=(const Matrix4x4& rhs){
        memcpy(matrix, rhs.matrix, sizeof(float)*16);
        return *this;
    }
    
    Matrix4x4& Matrix4x4(const Matrix4x4& rhs){
        memcpy(matrix, rhs.matrix, sizeof(float)*16);
        return *this;
    }
    
    void multRowCol(int row, int col, rhs, result[])
    {
        result[row][col] = 0.0f; 
        for(int i=row; i<4; ++i)
            for (int j=col; j<4; ++j)
                result[row*4+col] += matrix[i*4+j] * matrix[j*4+i]; 
    }
    
    Matrix4x4& operator* (const Matrix4x4& rhs){
        float result[16];
        for (int i=0; i<4; ++i)
            for (int j=0; j<4; ++j)
                multRowCol(i, j, rhs, result);
        memcpy(matrix, result, sizeof(float)*16);
        return *this;
    }
}
        template<typename T>
        struct SceneNode{
            T& data;
            long key;
            ScreenNode(T& node) : data(node), key(0){}
            bool operator< const(const SceneNode<T>& rhs) const{
                return key < rhs.key;
            }
            long getKey() const {
                return key;
            }
            void setKey(long key){
                this->key = key;
            }
        };
        template<typename T>
        struct TransformOperator{
            float matrix[16];
            std::svector<perands;
            
        }

#endif