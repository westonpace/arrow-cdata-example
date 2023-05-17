import pyarrow as pa
import MyModule

arr = pa.array([1, 2, 3])
print(MyModule.run_udf(arr))
