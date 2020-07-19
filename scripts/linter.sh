cpplint --recursive --linelength=100 \
  --filter=-runtime/references,-legal/copyright,-build/namespaces,-whitespace/parens,-build/include_order,-whitespace/indent,+build/c++14 $1
