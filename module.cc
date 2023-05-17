#include <arrow/array.h>
#include <arrow/array/builder_primitive.h>
#include <arrow/c/abi.h>
#include <arrow/c/bridge.h>
#include <arrow/result.h>
#include <pybind11/pybind11.h>

#include <memory>

void throw_not_ok(const arrow::Status& st) {
  if (!st.ok()) {
    throw pybind11::value_error("Unexpected error from arrow-c++: " +
                                st.ToString());
  }
}

std::shared_ptr<arrow::Array> from_pyarrow(const pybind11::handle& array) {
  if (!pybind11::hasattr(array, "_export_to_c")) {
    throw pybind11::type_error("Expected pyarrow.Array");
  }

  ArrowArray c_data_array;
  ArrowSchema c_data_schema;

  intptr_t c_data_array_ptr = reinterpret_cast<intptr_t>(&c_data_array);
  intptr_t c_data_schema_ptr = reinterpret_cast<intptr_t>(&c_data_schema);

  auto export_fn = array.attr("_export_to_c");
  export_fn(c_data_array_ptr, c_data_schema_ptr);

  arrow::Result<std::shared_ptr<arrow::Array>> maybe_arr =
      arrow::ImportArray(&c_data_array, &c_data_schema);

  throw_not_ok(maybe_arr.status());

  return maybe_arr.MoveValueUnsafe();
}

pybind11::object to_pyarrow(const arrow::Array& array) {
  ArrowArray c_data_array;
  ArrowSchema c_data_schema;

  throw_not_ok(arrow::ExportArray(array, &c_data_array, &c_data_schema));

  pybind11::module pyarrow = pybind11::module::import("pyarrow");
  pybind11::object pa_array_cls = pyarrow.attr("Array");
  auto import_fn = pa_array_cls.attr("_import_from_c");

  intptr_t c_data_array_ptr = reinterpret_cast<intptr_t>(&c_data_array);
  intptr_t c_data_schema_ptr = reinterpret_cast<intptr_t>(&c_data_schema);

  return import_fn(c_data_array_ptr, c_data_schema_ptr);
}

std::shared_ptr<arrow::Array> transform(const arrow::Array& input) {
  if (input.type_id() != arrow::Type::INT64) {
    throw pybind11::value_error("Expected an array of type int64");
  }
  const auto& int64_array = dynamic_cast<const arrow::Int64Array&>(input);
  const int64_t* int64_values = int64_array.raw_values();
  arrow::Int64Builder builder;
  throw_not_ok(builder.Reserve(int64_array.length()));
  for (int64_t i = 0; i < int64_array.length(); i++) {
    throw_not_ok(builder.Append(int64_values[i] * 2));
  }
  arrow::Result<std::shared_ptr<arrow::Array>> doubled = builder.Finish();
  throw_not_ok(doubled.status());
  return doubled.MoveValueUnsafe();
}

pybind11::object run_udf(const pybind11::handle& array) {
  std::shared_ptr<arrow::Array> arrow_cpp_array = from_pyarrow(array);
  std::shared_ptr<arrow::Array> doubled_array = transform(*arrow_cpp_array);
  return to_pyarrow(*doubled_array);
}

PYBIND11_MODULE(MyModule, m) {
  m.doc() = "An example module";
  m.def("run_udf", &run_udf,
        "A function that does some transformation of a pyarrow array");
}
