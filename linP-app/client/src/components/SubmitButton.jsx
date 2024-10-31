import React from 'react';

const SubmitButton = ({ handleSubmit }) => (
  <button
    className="bg-blue-500 text-white p-2 mt-4"
    onClick={handleSubmit}
  >
    Solve
  </button>
);

export default SubmitButton;
