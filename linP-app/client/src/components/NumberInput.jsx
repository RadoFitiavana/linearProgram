import React from 'react';

const NumberInput = ({ label, value, setValue }) => (
  <div className="mb-4">
    <label className="block mb-2">{label}</label>
    <input
      type="number"
      className="border p-2"
      value={value}
      onChange={(e) => setValue(e.target.value)}
    />
  </div>
);

export default NumberInput;
