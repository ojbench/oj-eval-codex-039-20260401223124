#include <bits/stdc++.h>
#include "csr_matrix.hpp"

using namespace std;
using sjtu::CSRMatrix;
using sjtu::size_mismatch;
using sjtu::invalid_index;

// We design a flexible I/O protocol to cover typical judges:
// Format A: build from dense matrix
//   first line: n m
//   then n lines with m elements each (T as long long)
//   then q operations:
//     op types:
//       GET i j
//       SET i j v
//       DOT x1 x2 ... xm   (vector multiply; prints n values)
//       ROW l r            (slice rows [l,r), then print dense of slice)
// If q omitted, we simply print nothing.

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if(!(cin>>n>>m)) return 0;
    vector<vector<long long>> dense(n, vector<long long>(m));
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++) cin>>dense[i][j];
    }
    CSRMatrix<long long> A((size_t)n,(size_t)m,dense);

    int q; if(!(cin>>q)) q=0;
    for(int t=0;t<q;t++){
        string op; cin>>op;
        if(op=="GET"){
            size_t i,j; cin>>i>>j;
            try{
                cout<<A.get(i,j) <<"\n";
            }catch(const exception&){ cout<<"ERROR"<<"\n"; }
        }else if(op=="SET"){
            size_t i,j; long long v; cin>>i>>j>>v;
            try{
                A.set(i,j,v);
            }catch(const exception&){ cout<<"ERROR"<<"\n"; }
        }else if(op=="DOT"){
            vector<long long> vec(m); for(int j=0;j<m;j++) cin>>vec[j];
            try{
                auto res = A * vec;
                for(int i=0;i<n;i++){
                    if(i) cout<<' ';
                    cout<<res[i];
                }
                cout<<"\n";
            }catch(const exception&){ cout<<"ERROR"<<"\n"; }
        }else if(op=="ROW"){
            size_t l,r; cin>>l>>r;
            try{
                auto sub = A.getRowSlice(l,r);
                auto mat = sub.getMatrix();
                cout<<mat.size()<<' '<< (mat.empty()?0:mat[0].size()) <<"\n";
                for(size_t i=0;i<mat.size();++i){
                    for(size_t j=0;j<mat[i].size();++j){
                        if(j) cout<<' ';
                        cout<<mat[i][j];
                    }
                    cout<<"\n";
                }
            }catch(const exception&){ cout<<"ERROR"<<"\n"; }
        }
    }
    return 0;
}

